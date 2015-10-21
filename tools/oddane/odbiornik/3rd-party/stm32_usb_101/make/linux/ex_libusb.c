#include <libusb-1.0/libusb.h>
#include <stdio.h>

static void error(char const *, int);
static int init_usb(void);
static int find_device(libusb_context *);
static int check_device(libusb_device *);
static int use_device(libusb_device_handle *);

int main() {
  return init_usb();
}

void error(char const *function_name, int error_code) {
  fprintf(stderr, "Error in %s, error code %d\n", function_name, error_code);
}

int init_usb() {
  libusb_context *context;
  int result;

  result = libusb_init(&context);
  if (result) {
    error("libusb_init", result);
  }
  else {
    result = find_device(context);
    libusb_exit(context);
  }
  return result;
}

int find_device(libusb_context *context) {
  libusb_device **device_list, *device;
  libusb_device_handle *handle;
  ssize_t count, i;
  int result;

  count = libusb_get_device_list(context, &device_list);
  if (count < 0) {
    error("libusb_get_device_list", count);
    return count;
  }

  for (device = NULL, i = 0; i < count; i++) {
    if (check_device(device_list[i])) {
      device = device_list[i];
      break;
    }
  }

  if (device)
    result = libusb_open(device, &handle);
  else
    result = 1;

  if (result < 0)
    error("libusb_open", result);

  libusb_free_device_list(device_list, 1);

  if (result == 0) {
    result = use_device(handle);
    libusb_close(handle);
  }

  return result;
}

static struct libusb_device_descriptor device_descriptor;

int check_device(libusb_device *device) {
  int result;

  result = libusb_get_device_descriptor(device, &device_descriptor);
  if (result) {
    error("libusb_get_device_descriptor", result);
    return 0;
  }
  else if (device_descriptor.idVendor == 0x0483)
    return 1;
  else
    return 0;
}

int use_device(libusb_device_handle *handle) {
  int result, i;

  printf("Device %04hx:%04hx found.\n",
         device_descriptor.idVendor, device_descriptor.idProduct);

  result = 0;
  for (i = -1; i <= device_descriptor.bNumConfigurations; ++i) {
    result = libusb_set_configuration(handle, i);
    if (result < 0) {
      error("libusb_set_configuration", result);
      break;
    }
    else {
      printf("Configuration %d set.\n", i);
    }
  }

  return result;
}
