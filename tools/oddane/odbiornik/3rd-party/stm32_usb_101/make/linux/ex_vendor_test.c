#include <libusb-1.0/libusb.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <usb_def.h>
#include <sys/time.h>

#define CNT_BUFF_SIZE           80
#define INT_BUFF_SIZE           48
#define ISO_BUFF_SIZE           40
#define BLK_BUFF_SIZE           64
#define INT_PCKT_COUNT          2
#define ISO_PCKT_COUNT          100
#define BLK_PCKT_COUNT          100
#define CNT_TBL_LENGTH          6
#define INT_TBL_LENGTH          5
#define CONTROL_TIMEOUT_MS      256
#define INTERRUPT_TIMEOUT_MS    256
#define BULK_TIMEOUT_MS         1000
#define ISOCHRONOUS_TIMEOUT_MS  500

#define SET_VALUE  3
#define GET_DATA   4
#define SET_DATA   5

/* PC and USB are little-endian. */
#define HTOUSBS(x) libusb_cpu_to_le16(x)
#define USBTOHS(x) libusb_le16_to_cpu(x)
#define HTOUSBL(x) (x)
#define USBTOHL(x) (x)

static void time_diff(struct timespec *, struct timespec *, long *);

static int control_write(libusb_device_handle *, uint16_t, uint16_t, uint8_t, long *);
static int control_read(libusb_device_handle *, uint16_t, uint16_t, uint8_t, long *);
static int interrupt_write(libusb_device_handle *, uint8_t, int, uint8_t, long *);
static int interrupt_read(libusb_device_handle *, uint8_t, int, uint8_t, long *);
static int bulk_write(libusb_device_handle *, uint8_t, int, uint8_t, long *);
static int bulk_read(libusb_device_handle *, uint8_t, int, uint8_t, long *);

static void print_status(enum libusb_transfer_status);
static void transfer_call_back(struct libusb_transfer *);
static int isochronous_write(libusb_context *, libusb_device_handle *,
                             uint8_t, int, uint8_t, long *);
static int isochronous_read(libusb_context *, libusb_device_handle *,
                            uint8_t, int, int, uint8_t, long *);

typedef enum {INIT, OUT, IN, PRINT} stage_t;

static void time_report(stage_t, int, long);
static void do_tests(libusb_context *, libusb_device_handle *, int);

/* Initialize the library, find a function device, communicate with
   the device, and finaly free the library. */
int main(int argc, char *args[]) {
  libusb_context       *context;
  libusb_device_handle *handle;
  int                  result, test_count;
  uint16_t             vid, pid;

  if (argc == 3) {
    result = sscanf(args[1], "%hx:%hx", &vid, &pid);
    test_count = atoi(args[2]);
  }
  else {
    result = 0;
    test_count = 0;
  }
  if (result != 2) {
    fprintf(stderr, "Usage: %s vid:pid test_count\n", args[0]);
    return 1;
  }

  result = libusb_init(&context);
  if (result < 0) {
    fprintf(stderr, "libusb_init failed, result %d\n", result);
    return 1;
  }

  libusb_set_debug(context, 3);

  handle = libusb_open_device_with_vid_pid(context, vid, pid);
  if (handle == 0) {
    fprintf(stderr, "libusb_open_device_with_vid_pid %04hx:%04hx failed\n",
            vid, pid);
    libusb_exit(context);
    return 2;
  }

  result = libusb_set_configuration(handle, 1);
  if (result < 0) {
    fprintf(stderr, "libusb_set_configuration failed\n");
    libusb_close(handle);
    libusb_exit(context);
    return 3;
  }

  result = libusb_claim_interface(handle, 0);
  if (result < 0) {
    fprintf(stderr, "libusb_claim_interface failed\n");
    libusb_close(handle);
    libusb_exit(context);
    return 4;
  }

  do_tests(context, handle, test_count);

  result = libusb_release_interface(handle, 0);
  if (result < 0) {
    fprintf(stderr, "libusb_release_interface failed\n");
    libusb_close(handle);
    libusb_exit(context);
    return 5;
  }

  libusb_close(handle);
  libusb_exit(context);
  return 0;
}

void time_diff(struct timespec *t1, struct timespec *t2, long *time) {
  if (time)
    *time = (t2->tv_sec - t1->tv_sec) * 1000000000 +
            t2->tv_nsec - t1->tv_nsec;
}

/** CONTROL TRANSFERS **/

int control_write(libusb_device_handle *handle, uint16_t value,
                  uint16_t length, uint8_t fill, long *time) {
  static uint8_t buffer[CNT_BUFF_SIZE];
  struct timespec t1, t2;
  int i, result;

  if (length > (int)(sizeof buffer)) {
    fprintf(stderr, "control_write, length %d too big\n", length);
    return 0;
  }
  for (i = 0; i < sizeof buffer; ++i)
    buffer[i] = fill + i;
  clock_gettime(CLOCK_REALTIME, &t1);
  result = libusb_control_transfer(handle,
                                   HOST_TO_DEVICE | VENDOR_REQUEST | INTERFACE_RECIPIENT,
                                   length == 0 ? SET_VALUE : SET_DATA,
                                   value, 0, buffer, length,
                                   CONTROL_TIMEOUT_MS);
  clock_gettime(CLOCK_REALTIME, &t2);
  time_diff(&t1, &t2, time);
  if (result < 0) {
    fprintf(stderr, "out libusb_control_transfer failed, "
            "result %d, value %d, length %d\n",
            result, value, length);
    return 0;
  }
  return 1;
}

int control_read(libusb_device_handle *handle, uint16_t value,
                 uint16_t length, uint8_t fill, long *time) {
  static uint8_t buffer[CNT_BUFF_SIZE];
  struct timespec t1, t2;
  int i, result;
  uint8_t expected;

  if (length > (int)(sizeof buffer)) {
    fprintf(stderr, "control_read, length %d too big\n", length);
    return 0;
  }
  for (i = 0; i < sizeof buffer; ++i)
    buffer[i] = 0;
  clock_gettime(CLOCK_REALTIME, &t1);
  result = libusb_control_transfer(handle,
                                   DEVICE_TO_HOST | VENDOR_REQUEST | INTERFACE_RECIPIENT,
                                   GET_DATA, value, 0, buffer, length,
                                   CONTROL_TIMEOUT_MS);
  clock_gettime(CLOCK_REALTIME, &t2);
  time_diff(&t1, &t2, time);
  if (result < 0) {
    fprintf(stderr, "in libusb_control_transfer failed, "
            "result %d, value %d, length %d\n",
            result, value, length);
    return 0;
  }
  for (i = 0; i < length; ++i) {
    expected = fill + i;
    if (expected != buffer[i]) {
      fprintf(stderr, "in libusb_control_transfer failed, wrong "
              "byte at position %d , expected %02x, received %02x\n",
              i, expected, buffer[i]);
      return 0;
    }
  }
  return 1;
}

/** INTERRUPT TRANSFERS **/

int interrupt_write(libusb_device_handle *handle, uint8_t ep,
                    int length, uint8_t fill, long *time) {
  static uint8_t buffer[INT_PCKT_COUNT * INT_BUFF_SIZE];
  struct timespec t1, t2;
  int i, transferred, result;

  if (length > (int)(sizeof buffer)) {
    fprintf(stderr, "interrupt_write, length %d too big\n", length);
    return 0;
  }
  for (i = 0; i < sizeof buffer; ++i)
    buffer[i] = fill + i;
  transferred = -1;
  clock_gettime(CLOCK_REALTIME, &t1);
  result = libusb_interrupt_transfer(handle, ep | ENDP_OUT, buffer,
                                     length, &transferred,
                                     INTERRUPT_TIMEOUT_MS);
  clock_gettime(CLOCK_REALTIME, &t2);
  time_diff(&t1, &t2, time);
  if (result < 0 || transferred != length) {
    fprintf(stderr, "out libusb_interrupt_transfer failed, "
            "result %d, length %d, transferred %d\n",
            result, length, transferred);
    return 0;
  }
  return 1;
}

int interrupt_read(libusb_device_handle *handle, uint8_t ep,
                   int length, uint8_t fill, long *time) {
  static uint8_t buffer[INT_PCKT_COUNT * INT_BUFF_SIZE];
  struct timespec t1, t2;
  int i, transferred, result;
  uint8_t expected;

  if (length > (int)(sizeof buffer)) {
    fprintf(stderr, "interrupt_read, length %d too big\n", length);
    return 0;
  }
  for (i = 0; i < sizeof buffer; ++i)
    buffer[i] = 0;
  transferred = -1;
  clock_gettime(CLOCK_REALTIME, &t1);
  result = libusb_interrupt_transfer(handle, ep | ENDP_IN, buffer,
                                     length, &transferred,
                                     INTERRUPT_TIMEOUT_MS);
  clock_gettime(CLOCK_REALTIME, &t2);
  time_diff(&t1, &t2, time);
  if (result < 0 || transferred != length) {
    fprintf(stderr, "in libusb_interrupt_transfer failed, "
            "result %d, length %d, transferred %d\n",
            result, length, transferred);
    return 0;
  }
  for (i = 0; i < length; ++i) {
    expected = fill + i;
    if (expected != buffer[i]) {
      fprintf(stderr, "in libusb_interrupt_transfer failed, wrong "
              "byte at position %d , expected %02x, received %02x\n",
              i, expected, buffer[i]);
      return 0;
    }
  }
  return 1;
}

/** BULK TRANSFERS **/

int bulk_write(libusb_device_handle *handle, uint8_t ep,
               int length, uint8_t fill, long *time) {
  static uint8_t buffer[BLK_PCKT_COUNT * BLK_BUFF_SIZE];
  struct timespec t1, t2;
  int i, transferred, total_transferred, result;

  if (length > sizeof buffer) {
    fprintf(stderr, "bulk_write, length %d too big\n", length);
    return 0;
  }
  for (i = 0; i < sizeof buffer; ++i)
    buffer[i] = fill + i;
  transferred = -1;
  total_transferred = 0;
  result = 0;
  clock_gettime(CLOCK_REALTIME, &t1);
  while (total_transferred < length && result >= 0) {
    result = libusb_bulk_transfer(handle, ep | ENDP_OUT,
                                  buffer + total_transferred,
                                  length - total_transferred,
                                  &transferred, BULK_TIMEOUT_MS);
    total_transferred += transferred;
  }
  clock_gettime(CLOCK_REALTIME, &t2);
  time_diff(&t1, &t2, time);
  if (result < 0 || total_transferred != length) {
    fprintf(stderr, "out libusb_bulk_transfer failed, "
            "result %d, length %d, transferred %d\n",
            result, length, transferred);
    return 0;
  }
  return 1;
}

int bulk_read(libusb_device_handle *handle, uint8_t ep,
              int length, uint8_t fill, long *time) {
  static uint8_t buffer[BLK_PCKT_COUNT * BLK_BUFF_SIZE];
  struct timespec t1, t2;
  int i, transferred, total_transferred, result;
  uint8_t expected;

  if (length > (int)(sizeof buffer)) {
    fprintf(stderr, "bulk_read, length %d too big\n", length);
    return 0;
  }
  for (i = 0; i < sizeof buffer; ++i)
    buffer[i] = 0;
  transferred = -1;
  total_transferred = 0;
  result = 0;
  clock_gettime(CLOCK_REALTIME, &t1);
  while (total_transferred < length && result >= 0) {
    result = libusb_bulk_transfer(handle, ep | ENDP_IN,
                                  buffer + total_transferred,
                                  length - total_transferred,
                                  &transferred, BULK_TIMEOUT_MS);
    total_transferred += transferred;
  }
  clock_gettime(CLOCK_REALTIME, &t2);
  time_diff(&t1, &t2, time);
  if (result < 0 || total_transferred != length) {
    if (length >= 0) {
      fprintf(stderr, "in libusb_bulk_transfer failed,"
              " result %d, length %d, transferred %d\n",
              result, length, transferred);
    }
    return 0;
  }
  for (i = 0; i < length; ++i) {
    expected = fill + i;
    if (expected != buffer[i]) {
      fprintf(stderr, "in libusb_bulk_transfer failed, wrong "
              "byte at position %d , expected %02x, received %02x\n",
              i, expected, buffer[i]);
      return 0;
    }
  }
  return 1;
}

/** ISOCHRONOUS TRANSFERS **/

void print_status(enum libusb_transfer_status status) {
  switch (status) {
    case LIBUSB_TRANSFER_COMPLETED:
      /* printf("LIBUSB_TRANSFER_COMPLETED\n"); */
      break;
    case LIBUSB_TRANSFER_ERROR:
      printf("LIBUSB_TRANSFER_ERROR\n");
      break;
    case LIBUSB_TRANSFER_TIMED_OUT:
      printf("LIBUSB_TRANSFER_TIMED_OUT\n");
      break;
    case LIBUSB_TRANSFER_CANCELLED:
      printf("LIBUSB_TRANSFER_CANCELLED\n");
      break;
    case LIBUSB_TRANSFER_STALL:
      printf("LIBUSB_TRANSFER_STALL\n");
      break;
    case LIBUSB_TRANSFER_NO_DEVICE:
      printf("LIBUSB_TRANSFER_NO_DEVICE\n");
      break;
    case LIBUSB_TRANSFER_OVERFLOW:
      printf("LIBUSB_TRANSFER_OVERFLOW\n");
      break;
    default:
      printf("Something else\n");
  }
}

void transfer_call_back(struct libusb_transfer *transfer) {
  print_status(transfer->status);
}

int isochronous_write(libusb_context *context,
                      libusb_device_handle *device_handle,
                      uint8_t ep, int count,
                      uint8_t fill, long *time) {
  static uint8_t buffer[ISO_PCKT_COUNT * ISO_BUFF_SIZE];
  struct libusb_transfer* transfer_handle;
  struct timespec t1, t2;
  int result, i;

  if (count > ISO_PCKT_COUNT) {
    fprintf(stderr, "isochronous_write, "
            "the number of packets %d too big\n", count);
    return 0;
  }
  for (i = 0; i < sizeof buffer; ++i)
    buffer[i] = fill + i;
  transfer_handle = libusb_alloc_transfer(count);
  if (transfer_handle == NULL) {
    fprintf(stderr, "libusb_alloc_transfer failed\n");
    return 0;
  }
  libusb_fill_iso_transfer(transfer_handle, device_handle,
                           ep | ENDP_OUT, buffer, sizeof buffer,
                           count, transfer_call_back, NULL,
                           ISOCHRONOUS_TIMEOUT_MS);
  transfer_handle->flags = LIBUSB_TRANSFER_FREE_TRANSFER;
  libusb_set_iso_packet_lengths(transfer_handle, ISO_BUFF_SIZE);
  result = libusb_submit_transfer(transfer_handle);
  if (result != 0) {
    fprintf(stderr, "libusb_submit_transfer, result %d\n", result);
    return 0;
  }
  clock_gettime(CLOCK_REALTIME, &t1);
  result = libusb_handle_events(context);
  clock_gettime(CLOCK_REALTIME, &t2);
  time_diff(&t1, &t2, time);
  if (result != 0) {
    fprintf(stderr, "libusb_handle_events failed, result %d\n", result);
    return 0;
  }
  return 1;
}

int isochronous_read(libusb_context *context,
                     libusb_device_handle *device_handle,
                     uint8_t ep, int count, int expected_count,
                     uint8_t fill, long *time) {
  static uint8_t buffer[ISO_PCKT_COUNT * ISO_BUFF_SIZE];
  struct libusb_transfer* transfer_handle;
  struct timespec t1, t2;
  int result, packet, packet_length, buffer_index, data_index, index;
  uint8_t expected;

  if (count > ISO_PCKT_COUNT) {
    fprintf(stderr, "isochronous_read, "
            "the number of packets %d too big\n", count);
    return 0;
  }
  transfer_handle = libusb_alloc_transfer(count);
  if (transfer_handle == NULL) {
    fprintf(stderr, "libusb_alloc_transfer failed\n");
    return 0;
  }
  libusb_fill_iso_transfer(transfer_handle, device_handle,
                           ep | ENDP_IN, buffer, sizeof buffer,
                           count, transfer_call_back, NULL,
                           ISOCHRONOUS_TIMEOUT_MS);
  expected_count *= ISO_BUFF_SIZE;
  for (index = 0; index < sizeof buffer; ++index)
    buffer[index] = 0;
  transfer_handle->flags = LIBUSB_TRANSFER_FREE_TRANSFER;
  libusb_set_iso_packet_lengths(transfer_handle, ISO_BUFF_SIZE);
  result = libusb_submit_transfer(transfer_handle);
  if (result != 0) {
    fprintf(stderr, "libusb_submit_transfer, result %d\n", result);
    return 0;
  }
  clock_gettime(CLOCK_REALTIME, &t1);
  result = libusb_handle_events(context);
  clock_gettime(CLOCK_REALTIME, &t2);
  time_diff(&t1, &t2, time);

  if (result != 0) {
    fprintf(stderr, "libusb_handle_events failed, result %d\n", result);
    return 0;
  }
  for (buffer_index = 0, packet = 0, data_index = 0; packet < count; ++packet) {
    print_status(transfer_handle->iso_packet_desc[packet].status);
    packet_length = transfer_handle->iso_packet_desc[packet].actual_length;
    for (index = 0; index < packet_length; ++index, ++data_index) {
      expected = fill + data_index;
      if (expected != buffer[buffer_index + index]) {
        fprintf(stderr, "in isochronous transfer failed, wrong byte "
                "at position %d, expected %02x, received %02x\n",
                buffer_index + index, expected,
                buffer[buffer_index + index]);
        return 0;
      }
    }
    buffer_index += transfer_handle->iso_packet_desc[packet].length;
  }
  if (data_index != expected_count) {
    fprintf(stderr, "in isochronous transfer failed, "
            "wrong byte count, received %d, expected %d\n",
            data_index, expected_count);
    return 0;
  }
  return 1;
}

/** TESTS **/

void time_report(stage_t stage, int result, long time) {
  static long outmin, outavg, outmax, inmin, inavg, inmax;
  static int testcount, outresult, inresult, outcount, incount;

  switch (stage) {
    case INIT:
      outmin = inmin = time * 1000000;
      outavg = inavg = 0;
      outmax = inmax = 0;
      testcount = result;
      outresult = inresult = 0;
      outcount = incount = 0;
      break;
    case OUT:
      outresult += result;
      if (result && time > 0) {
        ++outcount;
        if (time < outmin)
          outmin = time;
        outavg += time;
        if (time > outmax)
          outmax = time;
      }
      break;
    case IN:
      inresult += result;
      if (result && time > 0) {
        ++incount;
        if (time < inmin)
          inmin = time;
        inavg += time;
        if (time > inmax)
          inmax = time;
      }
      break;
    case PRINT:
      if (outresult != 0) {
        outavg += outcount / 2;
        if (outcount != 0)
          outavg /= outcount;
        else
          outavg = 0;
      }
      else {
        outmin = 0;
        outavg = 0;
      }
      if (inresult != 0) {
        inavg += incount / 2;
        if (incount != 0)
          inavg /= incount;
        else
          inavg = 0;
      }
      else {
        inmin = 0;
        inavg = 0;
      }
      printf("OUT: %d pass, %d fail, time min/avg/max %ld/%ld/%ld ns\n"
             "IN:  %d pass, %d fail, time min/avg/max %ld/%ld/%ld ns\n",
             outresult, testcount - outresult, outmin, outavg, outmax,
             inresult, testcount - inresult, inmin, inavg, inmax);
      break;
  }
}

void do_tests(libusb_context *context, libusb_device_handle *handle,
              int test_count) {
  static const uint8_t cnt_fill_tbl[CNT_TBL_LENGTH] = {
    0xaa, 0x55, 0x12, 0xcd, 0xff, 0x33
  };
  static const int cnt_length_tbl[CNT_TBL_LENGTH] = {
    31, 32, 33, 64, 7, 0
  };
  static const uint8_t int_fill_tbl[INT_TBL_LENGTH] = {
    0x55, 0xaa, 0x5a, 0x33, 0xcc
  };
  static const int int_length_tbl[INT_TBL_LENGTH] = {
    1, 96, 2, 48, 3
  };
  int i, result, blk_transfer;
  long time;
  uint8_t fill;

  time = 0;

  printf("\nControl buffer test\n"
         "Warning: one OUT and two IN tests should fail.\n");
  time_report(INIT, CNT_TBL_LENGTH, CONTROL_TIMEOUT_MS);
  for (i = 0; i < CNT_TBL_LENGTH; ++i) {
    /* The test with i == 4 fails because wValue is too large. */
    result = control_write(handle, i, cnt_length_tbl[i],
                           cnt_fill_tbl[i], &time);
    time_report(OUT, result, time);
  }
  for (i = 0; i < CNT_TBL_LENGTH; ++i) {
    /* The test with i == 4 fails because wValue is too large.
       The test with i == 5 fails because if wLength is zero then
       only OUT control transfer is possible. */
    result = control_read(handle, i, cnt_length_tbl[i],
                          cnt_fill_tbl[i], &time);
    time_report(IN, result, time);
  }
  time_report(PRINT, 0, 0);

  printf("\nInterrupt buffer test\n"
         "Warning: one IN test should fail.\n");
  time_report(INIT, INT_TBL_LENGTH, INTERRUPT_TIMEOUT_MS);
  for (i = 0; i < INT_TBL_LENGTH; ++i) {
    /* The last call causes buffer overflow. */
    result = interrupt_write(handle, ENDP1, int_length_tbl[i],
                             int_fill_tbl[i], &time);
    time_report(OUT, result, time);
  }
  for (i = 0; i < INT_TBL_LENGTH; ++i) {
    /* The last call fails because of buffer overflow. */
    result = interrupt_read(handle,  ENDP1, int_length_tbl[i],
                            int_fill_tbl[i], &time);
    time_report(IN, result, time);
  }
  time_report(PRINT, 0, 0);

  printf("\nInterrupt response time test, data size %d bytes\n",
         INT_BUFF_SIZE);
  time_report(INIT, test_count, INTERRUPT_TIMEOUT_MS);
  for (i = 0; i < test_count; ++i) {
    fill = i & 1 ? 0x55 : 0xaa;
    result = interrupt_write(handle, ENDP1, INT_BUFF_SIZE, fill, &time);
    time_report(OUT, result, time);
    result = interrupt_read(handle, ENDP1, INT_BUFF_SIZE, fill, &time);
    time_report(IN, result, time);
  }
  time_report(PRINT, 0, 0);

  printf("\nIsochronous transfer rate test, data size %d bytes, "
         "sent packets %d, received packets %d\n",
         ISO_BUFF_SIZE, ISO_PCKT_COUNT - 2, ISO_PCKT_COUNT);
  time_report(INIT, test_count, ISOCHRONOUS_TIMEOUT_MS);
  for (i = 0; i < test_count; ++i) {
    fill = i;
    result = isochronous_write(context, handle, ENDP2,
                               ISO_PCKT_COUNT - 2, fill, &time);
    time_report(OUT, result, time);
    result = isochronous_read(context, handle, ENDP2, ISO_PCKT_COUNT,
                              ISO_PCKT_COUNT - 2, fill, &time);
    time_report(IN, result, time);
  }
  time_report(PRINT, 0, 0);

  printf("\nBulk transfer rate test, data size %d bytes\n",
         BLK_BUFF_SIZE);
  time_report(INIT, test_count, BULK_TIMEOUT_MS);
  for (i = 0; i < test_count; ++i) {
    fill = i & 1 ? 0x55 : 0xaa;
    result = bulk_write(handle, ENDP3, BLK_BUFF_SIZE, fill, &time);
    time_report(OUT, result, time);
    result = bulk_read(handle, ENDP3, BLK_BUFF_SIZE, fill, &time);
    time_report(IN, result, time);
  }
  time_report(PRINT, 0, 0);

  blk_transfer = BLK_PCKT_COUNT * BLK_BUFF_SIZE;
  printf("\nBulk transfer rate test, data size %d bytes\n",
         blk_transfer);
  time_report(INIT, test_count, BULK_TIMEOUT_MS);
  for (i = 0; i < test_count; ++i) {
    fill = i & 1 ? 0x55 : 0xaa;
    result = bulk_write(handle, ENDP3, blk_transfer, fill, &time);
    time_report(OUT, result, time);
    result = bulk_read(handle, ENDP3, blk_transfer, fill, &time);
    time_report(IN, result, time);
  }
  time_report(PRINT, 0, 0);

  printf("\n");
}
