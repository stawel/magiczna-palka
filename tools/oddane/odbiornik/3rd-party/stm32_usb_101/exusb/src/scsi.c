#include <auxiliary.h>
#include <scsi.h>
#include <stm322xg_extsram.h>
#include <usb_def.h>

/* Switch logging on or off. */
#if 1
  #include <lcd_util.h>

  static void LOG(char const *msg, uint8_t const *cb, unsigned len) {
    unsigned i;

    LCDwriteWrap(msg);
    for (i = 0; i < len; ++i)
      LCDwriteHexWrap(2, cb[i]);
    LCDputcharWrap(' ');
  }
#else
  static void LOG(char const *msg, uint8_t const *cb, unsigned len) {
  }
#endif

/* SCSI command coding uses the big-endian order. */
#if 0
  #include <stm32.h>

  static uint32_t REVERSE_UINT32(uint32_t x) {
    return __REV(x);
  }

  static uint16_t REVERSE_UINT16(uint16_t x) {
    return __REV16(x);
  }
#else
  #define REVERSE_UINT32(x) ((((x) << 24) & 0xff000000U) | \
                             (((x) <<  8) & 0x00ff0000U) | \
                             (((x) >>  8) & 0x0000ff00U) | \
                             (((x) >> 24) & 0x000000ffU))
  #define REVERSE_UINT16(x) ((((x) << 8) & 0xff00U) | \
                             (((x) >> 8) & 0x00ffU))
#endif

/* Only very small subset of SCSI commands is supported. */
#define SCSI_TEST_UNIT_READY               0x00
#define SCSI_REQUEST_SENSE                 0x03
#define SCSI_INQUIRY                       0x12
#define SCSI_MODE_SENSE6                   0x1A
#define SCSI_MODE_SENSE10                  0x5A
#define SCSI_START_STOP_UNIT               0x1B
#define SCSI_PREVENT_ALLOW_MEDIUM_REMOVAL  0x1E
#define SCSI_READ_FORMAT_CAPACITIES        0x23
#define SCSI_READ_CAPACITY10               0x25
#define SCSI_READ10                        0x28
#define SCSI_WRITE10                       0x2A

/* SCSI sense keys */
#define SCSI_NO_SENSE         0x00
#define SCSI_RECOVERED_ERROR  0x01
#define SCSI_NOT_READY        0x02
#define SCSI_MEDIUM_ERROR     0x03
#define SCSI_HARDWARE_ERROR   0x04
#define SCSI_ILLEGAL_REQUEST  0x05
#define SCSI_UNIT_ATTENTION   0x06
#define SCSI_DATA_PROTECT     0x07
#define SCSI_BLANK_CHECK      0x08
#define SCSI_VENDOR_SPECIFIC  0x09
#define SCSI_COPY_ABORTED     0x0A
#define SCSI_ABORTED_COMMAND  0x0B
#define SCSI_VOLUME_OVERFLOW  0x0D
#define SCSI_MISCOMPARE       0x0E

typedef struct {
  uint8_t error_code;
  uint8_t obsolete;
  uint8_t key;
  uint8_t information[4];
  uint8_t add_length;
  uint8_t command_specific_information[4];
  uint8_t code;
  uint8_t qualifier;
  uint8_t specific[4];
} __packed scsi_request_sense6_data_t;

static scsi_request_sense6_data_t request_sense6 = {
  0x70, 0, SCSI_NO_SENSE, {0, 0, 0, 0}, sizeof(request_sense6) - 8,
  {0, 0, 0, 0}, 0, 0, {0, 0, 0, 0}
};

typedef struct {
  uint8_t pq_pdt;
  uint8_t rmb;
  uint8_t version;
  uint8_t format;
  uint8_t add_length;
  uint8_t flags[3];
  char    vendor[8];
  char    product[16];
  char    revision[4];
} __packed scsi_inquiry_data_t;

static const scsi_inquiry_data_t inquiry = {
  0x00, /* SCSI block command direct access device */
  0x80, /* removable medium */
  0x00, /* no conformance to any standard */
  0x02, /* response data format */
  sizeof(scsi_inquiry_data_t) - 5,
  {0, 0, 0},
  {'M', 'P', ' ', ' ', ' ', ' ', ' ', ' '},
  {'R', 'e', 'm', 'o', 'v', 'a', 'b', 'l',
   'e', ' ', 'm', 'e', 'm', 'o', 'r', 'y'},
  {'1', '.', '0' ,'0'}
};

typedef struct {
  uint8_t  pq_pdt;
  uint8_t  page_code;
  uint16_t page_length;
  char     serial_number[8];
} __packed scsi_unit_serial_number_data_t;

static const scsi_unit_serial_number_data_t serial_number = {
  0x00, /* SCSI block command direct access device */
  0x80, /* Unit Serial Number VPD page */
  REVERSE_UINT16(sizeof(scsi_unit_serial_number_data_t) - 4),
  {'0', '0', '0', '0', '0', '0', '0', '1'}
};

typedef struct {
  uint8_t mode_data_length;
  uint8_t medium_type;
  uint8_t device_specific_parameter;
  uint8_t block_descriptor_length;
} __packed scsi_mode_parameter_header6_t;

static const scsi_mode_parameter_header6_t mode_sense6 = {
  sizeof(scsi_mode_parameter_header6_t) - 1, 0, 0, 0
};

typedef struct {
  uint16_t mode_data_length;
  uint8_t  medium_type;
  uint8_t  device_specific_parameter;
  uint8_t  long_lba;
  uint8_t  reserverd;
  uint16_t block_descriptor_length;
} __packed scsi_mode_parameter_header10_t;

static const scsi_mode_parameter_header10_t mode_sense10 = {
  REVERSE_UINT16(sizeof(scsi_mode_parameter_header10_t) - 2),
  0, 0, 0, 0, 0
};

typedef struct {
  uint8_t  reserved[3];
  uint8_t  capacity_list_length;
  uint32_t lba_count; /* big-endian format */
  uint8_t  descriptor_code;
  uint8_t  block_length_msb;
  uint16_t block_length; /* big-endian format */
} __packed scsi_format_capacity_data_t;

static const scsi_format_capacity_data_t format_capacity = {
  {0, 0, 0},
  sizeof(scsi_format_capacity_data_t) - 4,
  REVERSE_UINT32(LB_COUNT),
  1, /* unformatted media - maximum formatable capacity */
  0, /* logical block shorter than 65536 bytes */
  REVERSE_UINT16(LB_SIZE)
};

typedef struct {
  uint32_t last_lba;     /* big-endian format */
  uint32_t block_length; /* big-endian format */
} __packed scsi_capacity_data_t;

static const scsi_capacity_data_t capacity = {
  REVERSE_UINT32(LB_COUNT - 1),
  REVERSE_UINT32(LB_SIZE)
};

/* This implementation is stateless. Only the last error code needs to
   be cleared. */
void SCSIreset() {
  request_sense6.key = SCSI_NO_SENSE;
  request_sense6.code = 0;
}

/* This implementation dooes not support multiple logical units. */
uint8_t SCSIgetMaxLUN() {
  return 0;
}

/* Process SCSI command.
    lun      - the logical unit number
    cb       - the pointer to the command block
    cb_len   - the command block length
    data     - the pointer to the data pointer
    data_len - the requested transfer size
   Return a value for the bCSWStatus field in the Command Status
   Wrapper. */
uint8_t SCSIcommand(uint8_t lun, uint8_t const *cb, uint8_t cb_len,
                    uint8_t **data, uint32_t *data_len,
                    scsi_direction_t *transfer_direction) {
  uint32_t alloc_len, lba, lbc;

  if (cb[0] == SCSI_INQUIRY && cb_len >= 6) {
    /* Windows violates the command length rule. */
  }
  else if ((cb[0] >= 0x00 && cb[0] <= 0x1f && cb_len != 6) ||
           (cb[0] >= 0x20 && cb[0] <= 0x5f && cb_len != 10) ||
           (cb[0] >= 0x80 && cb[0] <= 0x9f && cb_len != 16) ||
           (cb[0] >= 0xa0 && cb[0] <= 0xbf && cb_len != 12)) {
    LOG("R", cb, cb_len);
    request_sense6.key = SCSI_ILLEGAL_REQUEST;
    request_sense6.code = 0x0e; /* INVALID INFORMATION UNIT */
    *data = 0;
    *data_len = 0;
    return MSC_BOT_CSW_COMMAND_FAILED;
  }

  switch (cb[0]) {
    case SCSI_TEST_UNIT_READY:
      *data = 0;
      *data_len = 0;
      return MSC_BOT_CSW_COMMAND_PASSED;

    case SCSI_REQUEST_SENSE:
      alloc_len = cb[4];
      *data = (uint8_t *)&request_sense6;
      *data_len = min(alloc_len, sizeof(request_sense6));
      *transfer_direction = SCSI_IN;
      return MSC_BOT_CSW_COMMAND_PASSED;

    case SCSI_INQUIRY:
      /* The fields cb[3] and cb[4] contain allocation length in
         big-endian order. The lsb in the cb[1] field is evpd bit.
         If evpd is equal to 0 standard inquiry data is requested.
         If evpd is equal to 1 the cb[2] field specifies the requested
         page code. */
      alloc_len = cb[3] << 8 | cb[4];
      if (cb[1] == 0 || cb[2] == 0) {
        *data = (uint8_t *)&inquiry;
        *data_len = min(alloc_len, sizeof(inquiry));
        *transfer_direction = SCSI_IN;
        return MSC_BOT_CSW_COMMAND_PASSED;
      }
      else if (cb[1] == 1 || cb[2] == 0x80) {
        /* Windows requires a serial number. */
        *data = (uint8_t *)&serial_number;
        *data_len = min(alloc_len, sizeof(serial_number));
        *transfer_direction = SCSI_IN;
        return MSC_BOT_CSW_COMMAND_PASSED;
      }
      else {
        LOG("I", cb, cb_len);
        request_sense6.key = SCSI_ILLEGAL_REQUEST;
        request_sense6.code = 0x0e; /* INVALID INFORMATION UNIT */
        *data = 0;
        *data_len = 0;
        return MSC_BOT_CSW_COMMAND_FAILED;
      }

    case SCSI_MODE_SENSE6:
      alloc_len = cb[4];
      *data = (uint8_t *)&mode_sense6;
      *data_len = min(alloc_len, sizeof(mode_sense6));
      *transfer_direction = SCSI_IN;
      return MSC_BOT_CSW_COMMAND_PASSED;

    case SCSI_MODE_SENSE10:
      alloc_len = cb[7] << 8 | cb[8];
      *data = (uint8_t *)&mode_sense10;
      *data_len = min(alloc_len, sizeof(mode_sense10));
      *transfer_direction = SCSI_IN;
      return MSC_BOT_CSW_COMMAND_PASSED;

    case SCSI_START_STOP_UNIT:
      /* Windows sends this command before eject enabling. */
    case SCSI_PREVENT_ALLOW_MEDIUM_REMOVAL:
      /* Here is a place to write buffers, flush cache, etc. */
      *data = 0;
      *data_len = 0;
      return MSC_BOT_CSW_COMMAND_PASSED;

    case SCSI_READ_FORMAT_CAPACITIES:
      /* Windows only, if not handled then connecting takes long time. */
      *data = (uint8_t *)&format_capacity;
      *data_len = sizeof(format_capacity);
      *transfer_direction = SCSI_IN;
      return MSC_BOT_CSW_COMMAND_PASSED;

    case SCSI_READ_CAPACITY10:
      *data = (uint8_t *)&capacity;
      *data_len = sizeof(capacity);
      *transfer_direction = SCSI_IN;
      return MSC_BOT_CSW_COMMAND_PASSED;

    case SCSI_READ10:
    case SCSI_WRITE10:
      lba = cb[2] << 24 | cb[3] << 16 | cb[4] << 8 | cb[5];
      lbc = cb[7] << 8 | cb[8];
      if (lbc == 0 || lbc > LB_COUNT || lba > LB_COUNT - lbc) {
        LOG("T", cb, cb_len);
        request_sense6.key = SCSI_VOLUME_OVERFLOW;
        request_sense6.code = 0x21; /* LOGICAL BLOCK ADDRESS OUT OF RANGE */
        *data = 0;
        *data_len = 0;
        return MSC_BOT_CSW_COMMAND_FAILED;
      }
      else {
        *data = (uint8_t *)RAMDISK[lba];
        *data_len = lbc * LB_SIZE;
        if (cb[0] == SCSI_READ10)
          *transfer_direction = SCSI_IN;
        else
          *transfer_direction = SCSI_OUT;
        return MSC_BOT_CSW_COMMAND_PASSED;
      }

    default:
      LOG("C", cb, cb_len);
      request_sense6.key = SCSI_ILLEGAL_REQUEST;
      request_sense6.code = 0x20; /* INVALID COMMAND OPERATION CODE */
      *data = 0;
      *data_len = 0;
      return MSC_BOT_CSW_COMMAND_FAILED;
  }
}
