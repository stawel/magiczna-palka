#include <board_usb_def.h>
#include <error.h>
#include <pwr_periph.h>
#include <usb_endianness.h>
#include <usb_vid_pid.h>
#include <usbd_api.h>
#include <usbd_callbacks.h>
#include <usbd_power.h>

/** Descriptors **/

#define CNT_BUFF_SIZE  80
#define INT_BUFF_SIZE  48
#define ISO_BUFF_SIZE  40
#define BLK_BUFF_SIZE  64

static usb_device_descriptor_t const device_descriptor = {
  sizeof(usb_device_descriptor_t), /* bLength */
  DEVICE_DESCRIPTOR,               /* bDescriptorType */
  HTOUSBS(0x0200),                 /* bcdUSB */
  0x00,                            /* bDeviceClass */
  0x00,                            /* bDeviceSubClass */
  0x00,                            /* bDeviceProtocol */
  32,                              /* bMaxPacketSize0 */
  HTOUSBS(VID),                    /* idVendor */
  HTOUSBS(PID + 5),                /* idProduct */
  HTOUSBS(0x0110),                 /* bcdDevice */
  1,                               /* iManufacturer */
  2,                               /* iProduct */
  3,                               /* iSerialNumber */
  1                                /* bNumConfigurations */
};

typedef struct {
  usb_configuration_descriptor_t cnf_descr;
  usb_interface_descriptor_t     if_descr;
  usb_endpoint_descriptor_t      ep_descr[6];
} __packed usb_vendor_conf_t;

static usb_vendor_conf_t const vendor_conf = {
  {
    sizeof(usb_configuration_descriptor_t), /* bLength */
    CONFIGURATION_DESCRIPTOR,               /* bDescriptorType */
    HTOUSBS(sizeof(usb_vendor_conf_t)),     /* wTotalLength */
    1,                                      /* bNumInterfaces */
    1,                                      /* bConfigurationValue */
    4,                                      /* iConfiguration */
    USB_BM_ATTRIBUTES,                      /* bmAttributes */
    USB_B_MAX_POWER                         /* bMaxPower */
  },
  {
    sizeof(usb_interface_descriptor_t),     /* bLength */
    INTERFACE_DESCRIPTOR,                   /* bDescriptorType */
    0,                                      /* bInterfaceNumber */
    0,                                      /* bAlternateSetting */
    6,                                      /* bNumEndpoints */
    VENDOR_SPECIFIC,                        /* bInterfaceClass */
    VENDOR_SPECIFIC,                        /* bInterfaceSubClass */
    VENDOR_SPECIFIC,                        /* bInterfaceProtocol */
    4                                       /* iInterface */
  },
  {
    {
      sizeof(usb_endpoint_descriptor_t),    /* bLength */
      ENDPOINT_DESCRIPTOR,                  /* bDescriptorType */
      ENDP1 | ENDP_IN,                      /* bEndpointAddress */
      INTERRUPT_TRANSFER,                   /* bmAttributes */
      HTOUSBS(INT_BUFF_SIZE),               /* wMaxPacketSize */
      1                                     /* bInterval */
    },
    {
      sizeof(usb_endpoint_descriptor_t),    /* bLength */
      ENDPOINT_DESCRIPTOR,                  /* bDescriptorType */
      ENDP1 | ENDP_OUT,                     /* bEndpointAddress */
      INTERRUPT_TRANSFER,                   /* bmAttributes */
      HTOUSBS(INT_BUFF_SIZE),               /* wMaxPacketSize */
      1                                     /* bInterval */
    },
    {
      sizeof(usb_endpoint_descriptor_t),    /* bLength */
      ENDPOINT_DESCRIPTOR,                  /* bDescriptorType */
      ENDP2 | ENDP_IN,                      /* bEndpointAddress */
      ISOCHRONOUS_TRANSFER,                 /* bmAttributes */
      HTOUSBS(ISO_BUFF_SIZE),               /* wMaxPacketSize */
      1                                     /* bInterval */
    },
    {
      sizeof(usb_endpoint_descriptor_t),    /* bLength */
      ENDPOINT_DESCRIPTOR,                  /* bDescriptorType */
      ENDP2 | ENDP_OUT,                     /* bEndpointAddress */
      ISOCHRONOUS_TRANSFER,                 /* bmAttributes */
      HTOUSBS(ISO_BUFF_SIZE),               /* wMaxPacketSize */
      1                                     /* bInterval */
    },
    {
      sizeof(usb_endpoint_descriptor_t),    /* bLength */
      ENDPOINT_DESCRIPTOR,                  /* bDescriptorType */
      ENDP3 | ENDP_IN,                      /* bEndpointAddress */
      BULK_TRANSFER,                        /* bmAttributes */
      HTOUSBS(BLK_BUFF_SIZE),               /* wMaxPacketSize */
      0                                     /* bInterval */
    },
    {
      sizeof(usb_endpoint_descriptor_t),    /* bLength */
      ENDPOINT_DESCRIPTOR,                  /* bDescriptorType */
      ENDP3 | ENDP_OUT,                     /* bEndpointAddress */
      BULK_TRANSFER,                        /* bmAttributes */
      HTOUSBS(BLK_BUFF_SIZE),               /* wMaxPacketSize */
      0                                     /* bInterval */
    }
  }
};

static usb_string_descriptor_t(1) const string_lang = {
  sizeof(usb_string_descriptor_t(1)),
  STRING_DESCRIPTOR,
  {HTOUSBS(LANG_US_ENGLISH)}
};

static usb_string_descriptor_t(16) const string_manufacturer = {
  sizeof(usb_string_descriptor_t(16)),
  STRING_DESCRIPTOR,
  {
    HTOUSBS('M'), HTOUSBS('a'), HTOUSBS('r'), HTOUSBS('c'),
    HTOUSBS('i'), HTOUSBS('n'), HTOUSBS(' '), HTOUSBS('P'),
    HTOUSBS('e'), HTOUSBS('c'), HTOUSBS('z'), HTOUSBS('a'),
    HTOUSBS('r'), HTOUSBS('s'), HTOUSBS('k'), HTOUSBS('i')
  }
};

static usb_string_descriptor_t(41) const string_product = {
  sizeof(usb_string_descriptor_t(41)),
  STRING_DESCRIPTOR,
  {
    HTOUSBS('U'), HTOUSBS('S'), HTOUSBS('B'), HTOUSBS(' '),
    HTOUSBS('b'), HTOUSBS('u'), HTOUSBS('s'), HTOUSBS(' '),
    HTOUSBS('p'), HTOUSBS('o'), HTOUSBS('w'), HTOUSBS('e'),
    HTOUSBS('r'), HTOUSBS('e'), HTOUSBS('d'),
    HTOUSBS(' '), HTOUSBS('f'), HTOUSBS('u'), HTOUSBS('l'),
    HTOUSBS('l'), HTOUSBS(' '), HTOUSBS('s'), HTOUSBS('p'),
    HTOUSBS('e'), HTOUSBS('e'), HTOUSBS('d'), HTOUSBS(' '),
    HTOUSBS('d'), HTOUSBS('e'), HTOUSBS('v'), HTOUSBS('i'),
    HTOUSBS('c'), HTOUSBS('e'), HTOUSBS(' '), HTOUSBS('e'),
    HTOUSBS('x'), HTOUSBS('a'), HTOUSBS('m'), HTOUSBS('p'),
    HTOUSBS('l'), HTOUSBS('e')
  }
};

static usb_string_descriptor_t(10) const string_serial = {
  sizeof(usb_string_descriptor_t(10)),
  STRING_DESCRIPTOR,
  {
    HTOUSBS('0'), HTOUSBS('0'), HTOUSBS('0'), HTOUSBS('0'),
    HTOUSBS('0'), HTOUSBS('0'), HTOUSBS('0'), HTOUSBS('0'),
    HTOUSBS('0'), HTOUSBS('1')
  }
};

static usb_string_descriptor_t(9) const string_configuration = {
  sizeof(usb_string_descriptor_t(9)),
  STRING_DESCRIPTOR,
  {
    HTOUSBS('D'), HTOUSBS('a'), HTOUSBS('t'), HTOUSBS('a'),
    HTOUSBS(' '), HTOUSBS('e'), HTOUSBS('c'), HTOUSBS('h'),
    HTOUSBS('o')
  }
};

typedef struct {
  uint8_t const *data;
  uint16_t      length;
} string_table_t;

static string_table_t const strings[] = {
  {(uint8_t const*)&string_lang,          sizeof string_lang},
  {(uint8_t const*)&string_manufacturer,  sizeof string_manufacturer},
  {(uint8_t const*)&string_product,       sizeof string_product},
  {(uint8_t const*)&string_serial,        sizeof string_serial},
  {(uint8_t const*)&string_configuration, sizeof string_configuration}
};
static uint32_t const stringCount = sizeof(strings)/sizeof(strings[0]);

/** Endpoint buffers and transfer functions **/

#define SET_VALUE  3
#define GET_DATA   4
#define SET_DATA   5

#define BUFFER_COUNT0    4
#define BUFFER_COUNT1    4
#define BUFFER_COUNT2  112
#define BUFFER_COUNT3  112

static uint16_t size0[BUFFER_COUNT0];
static uint8_t  data0[BUFFER_COUNT0][CNT_BUFF_SIZE];
static uint16_t value;

static int      buffered1, rxidx1, txidx1, txbusy1;
static uint16_t size1[BUFFER_COUNT1];
static uint8_t  data1[BUFFER_COUNT1][INT_BUFF_SIZE];

static int      buffered2, rxidx2, txidx2;
static uint16_t size2[BUFFER_COUNT2];
static uint8_t  data2[BUFFER_COUNT2][ISO_BUFF_SIZE];

static int      buffered3, rxidx3, txidx3, txbusy3;
static uint16_t size3[BUFFER_COUNT3];
static uint8_t  data3[BUFFER_COUNT3][BLK_BUFF_SIZE];

static usb_result_t VendorNoDataSetup(usb_setup_packet_t const *);
static usb_result_t VendorInDataSetup(usb_setup_packet_t const *,
                                      uint8_t const **, uint16_t *);
static usb_result_t VendorOutDataSetup(usb_setup_packet_t const *,
                                       uint8_t **);
static void         VendorStatusIn(usb_setup_packet_t const *setup);
static void         EPinterruptRx(void);
static void         EPinterruptTx(void);
static void         EPisochronousRx(void);
static void         EPisochronousTx(void);
static void         EPbulkRx(void);
static void         EPbulkTx(void);

usb_result_t VendorNoDataSetup(usb_setup_packet_t const *setup) {
  if (setup->bmRequestType == (HOST_TO_DEVICE |
                               VENDOR_REQUEST |
                               INTERFACE_RECIPIENT) &&
      setup->bRequest == SET_VALUE &&
      setup->wIndex == 0 && /* Interface 0 */
      setup->wLength == 0) {
    value = setup->wValue;
    return REQUEST_SUCCESS;
  }
  return REQUEST_ERROR;
}

usb_result_t VendorInDataSetup(usb_setup_packet_t const *setup,
                               uint8_t const **data,
                               uint16_t *length) {
  if (setup->bmRequestType == (DEVICE_TO_HOST |
                               VENDOR_REQUEST |
                               INTERFACE_RECIPIENT) &&
      setup->bRequest == GET_DATA &&
      setup->wIndex == 0 && /* Interface 0 */
      setup->wValue < BUFFER_COUNT0  &&
      size0[setup->wValue] > 0) {
    *data = data0[setup->wValue];
    *length = size0[setup->wValue];
    return REQUEST_SUCCESS;
  }
  return REQUEST_ERROR;
}

usb_result_t VendorOutDataSetup(usb_setup_packet_t const *setup,
                                uint8_t **data) {
  if (setup->bmRequestType == (HOST_TO_DEVICE |
                               VENDOR_REQUEST |
                               INTERFACE_RECIPIENT) &&
      setup->bRequest == SET_DATA &&
      setup->wIndex == 0 && /* Interface 0 */
      setup->wValue < BUFFER_COUNT0  &&
      setup->wLength > 0 &&
      setup->wLength <= CNT_BUFF_SIZE) {
    *data = data0[setup->wValue];
    size0[setup->wValue] = 0;
    return REQUEST_SUCCESS;
  }
  return REQUEST_ERROR;
}

void VendorStatusIn(usb_setup_packet_t const *setup) {
  if (setup->bmRequestType == (HOST_TO_DEVICE |
                               VENDOR_REQUEST |
                               INTERFACE_RECIPIENT) &&
      setup->bRequest == SET_DATA &&
      setup->wIndex == 0 && /* Interface 0 */
      setup->wValue < BUFFER_COUNT0  &&
      setup->wLength > 0 &&
      setup->wLength <= CNT_BUFF_SIZE) {
    size0[setup->wValue] = setup->wLength;
  }
}

void EPinterruptRx() {
  if (buffered1 < BUFFER_COUNT1) {
    size1[rxidx1] = USBDread(ENDP1, data1[rxidx1], INT_BUFF_SIZE);
    ++buffered1;
    rxidx1 = rxidx1 < BUFFER_COUNT1 - 1 ? rxidx1 + 1 : 0;
  }
  else /* Enable receiver for the next transmission. */
    USBDread(ENDP1, 0, 0);
  if (!txbusy1)
    EPinterruptTx();
}

void EPinterruptTx() {
  if (buffered1 > 0) {
    USBDwrite(ENDP1, data1[txidx1], size1[txidx1]);
    -- buffered1;
    txidx1 = txidx1 < BUFFER_COUNT1 - 1 ? txidx1 + 1 : 0;
    txbusy1 = 1;
  }
  else
    txbusy1 = 0;
}

void EPisochronousRx() {
  if (buffered2 < BUFFER_COUNT2) {
    size2[rxidx2] = USBDread(ENDP2, data2[rxidx2], ISO_BUFF_SIZE);
    ++buffered2;
    rxidx2 = rxidx2 < BUFFER_COUNT2 - 1 ? rxidx2 + 1 : 0;
  }
}

void EPisochronousTx() {
  if (buffered2 > 0) {
    USBDwrite(ENDP2, data2[txidx2], size2[txidx2]);
    -- buffered2;
    txidx2 = txidx2 < BUFFER_COUNT2 - 1 ? txidx2 + 1 : 0;
  }
  else /* There is no data to send. Send empty packet. */
    USBDwrite(ENDP2, 0, 0);
}

void EPbulkRx() {
  if (buffered3 < BUFFER_COUNT3) {
    size3[rxidx3] = USBDread(ENDP3, data3[rxidx3], BLK_BUFF_SIZE);
    ++buffered3;
    rxidx3 = rxidx3 < BUFFER_COUNT3 - 1 ? rxidx3 + 1 : 0;
  }
  else /* Enable receiver for the next transmission. */
    USBDread(ENDP3, 0, 0);
  if (!txbusy3)
    EPbulkTx();
}

void EPbulkTx() {
  if (buffered3 > 0) {
    USBDwrite(ENDP3, data3[txidx3], size3[txidx3]);
    -- buffered3;
    txidx3 = txidx3 < BUFFER_COUNT3 - 1 ? txidx3 + 1 : 0;
    txbusy3 = 1;
  }
  else
    txbusy3 = 0;
}

/** Application callbacks **/

static int          Configure(void);
static uint8_t      Reset(usb_speed_t);
static usb_result_t GetDescriptor(uint16_t, uint16_t,
                                  uint8_t const **, uint16_t *);
static uint8_t      GetConfiguration(void);
static usb_result_t SetConfiguration(uint16_t);
static uint16_t     GetStatus(void);
static usb_result_t ClearDeviceFeature(uint16_t);
static usb_result_t SetDeviceFeature(uint16_t);
static void         Suspend(void);
static void         Wakeup(void);

static usbd_callback_list_t const ApplicationCallBacks = {
  Configure, Reset, 0, GetDescriptor, 0,
  GetConfiguration, SetConfiguration, GetStatus, 0, 0,
  ClearDeviceFeature, SetDeviceFeature,
  VendorNoDataSetup, VendorInDataSetup,
  VendorOutDataSetup, VendorStatusIn,
  {EPinterruptTx, EPisochronousTx, EPbulkTx,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {EPinterruptRx, EPisochronousRx, EPbulkRx,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  Suspend, Wakeup
};

/** Application initialization **/

static uint8_t configuration;

usbd_callback_list_t const * USBDgetApplicationCallbacks() {
  return &ApplicationCallBacks;
}

static void ResetState() {
  int i;

  for (i = 0; i < BUFFER_COUNT0; ++i)
    size0[i] = 0;
  value = 0;
  buffered1 = rxidx1 = txidx1 = txbusy1 = 0;
  buffered2 = rxidx2 = txidx2 = 0;
  buffered3 = rxidx3 = txidx3 = txbusy3 = 0;
  configuration = 0;
}

int Configure() {
  ResetState();
  PowerLEDconfigure();
  WakeupButtonConfigure();
  PWRsetRemoteWakeUp(RW_DISABLED);
  return 0;
}

uint8_t Reset(usb_speed_t speed) {
  ErrorResetable(speed == FULL_SPEED ? 0 : -1, 6);
  PWRsetRemoteWakeUp(RW_DISABLED);
  ResetState();

  /* Default control endpoint must be configured here. */
  if (USBDendPointConfigure(ENDP0, CONTROL_TRANSFER,
                            device_descriptor.bMaxPacketSize0,
                            device_descriptor.bMaxPacketSize0) !=
                                                      REQUEST_SUCCESS)
    ErrorResetable(-1, 7);

  /* USB standard says that suspend is alowed just in the powered
     state. Micro is to slow to enter suspend and then wake up before
     USB bus reset. Therefore we alow suspend in the default state. */
  PWRmanagementEnable();

  return device_descriptor.bMaxPacketSize0;
}

/** Standard request callbacks **/

usb_result_t GetDescriptor(uint16_t wValue, uint16_t wIndex,
                           uint8_t const **data, uint16_t *length) {
  uint32_t index = wValue & 0xff;

  switch (wValue >> 8) {
    case DEVICE_DESCRIPTOR:
      if (index == 0 && wIndex == 0) {
        *data = (uint8_t const *)&device_descriptor;
        *length = sizeof(usb_device_descriptor_t);
        return REQUEST_SUCCESS;
      }
      return REQUEST_ERROR;
    case CONFIGURATION_DESCRIPTOR:
      if (index == 0 && wIndex == 0) {
        *data = (uint8_t const *)&vendor_conf;
        *length = sizeof(usb_vendor_conf_t);
        return REQUEST_SUCCESS;
      }
      return REQUEST_ERROR;
    case STRING_DESCRIPTOR:
      if (index < stringCount) {
        *data = strings[index].data;
        *length = strings[index].length;
        return REQUEST_SUCCESS;
      }
      return REQUEST_ERROR;
    default:
      return REQUEST_ERROR;
  }
}

uint8_t GetConfiguration() {
  return configuration;
}

usb_result_t SetConfiguration(uint16_t confValue) {
  if (confValue <= device_descriptor.bNumConfigurations) {
    configuration = confValue;
    USBDdisableAllNonControlEndPoints();
    if (confValue == vendor_conf.cnf_descr.bConfigurationValue) {
      usb_result_t r1, r2, r3;
      r1 = USBDendPointConfigure(ENDP1, INTERRUPT_TRANSFER,
                                 INT_BUFF_SIZE, INT_BUFF_SIZE);
      r2 = USBDendPointConfigure(ENDP2, ISOCHRONOUS_TRANSFER,
                                 ISO_BUFF_SIZE, ISO_BUFF_SIZE);
      r3 = USBDendPointConfigure(ENDP3, BULK_TRANSFER,
                                 BLK_BUFF_SIZE, BLK_BUFF_SIZE);
      if (r1 == REQUEST_SUCCESS &&
          r2 == REQUEST_SUCCESS &&
          r3 == REQUEST_SUCCESS)
        return REQUEST_SUCCESS;
      else
        return REQUEST_ERROR;
    }
    return REQUEST_SUCCESS;
  }
  return REQUEST_ERROR;
}

uint16_t GetStatus() {
  uint16_t result = 0;

  /* Current power setting should be reported. */
  if (vendor_conf.cnf_descr.bmAttributes & SELF_POWERED)
    result |= STATUS_SELF_POWERED;
  if ((vendor_conf.cnf_descr.bmAttributes & REMOTE_WAKEUP) &&
      PWRgetRemoteWakeUp() == RW_ENABLED)
    result |= STATUS_REMOTE_WAKEUP;
  return result;
}

/** Power management callbacks **/

usb_result_t ClearDeviceFeature(uint16_t featureSelector) {
  if (featureSelector == DEVICE_REMOTE_WAKEUP &&
      (vendor_conf.cnf_descr.bmAttributes & REMOTE_WAKEUP)) {
    PWRsetRemoteWakeUp(RW_DISABLED);
    return REQUEST_SUCCESS;
  }
  return REQUEST_ERROR;
}

usb_result_t SetDeviceFeature(uint16_t featureSelector) {
  if (featureSelector == DEVICE_REMOTE_WAKEUP &&
      (vendor_conf.cnf_descr.bmAttributes & REMOTE_WAKEUP)) {
    PWRsetRemoteWakeUp(RW_ENABLED);
    return REQUEST_SUCCESS;
  }
  return REQUEST_ERROR;
}

static int green_led_state, red_led_state, power_led_state;

/* Switch peripherals off */
void Suspend() {
  green_led_state = GreenLEDstate();
  red_led_state   = RedLEDstate();
  power_led_state = PowerLEDstate();
  GreenLEDoff();
  RedLEDoff();
  PowerLEDoff();
}

/* Switch peripherals on */
void Wakeup() {
  if (green_led_state)
    GreenLEDon();
  if (red_led_state)
    RedLEDon();
  if (power_led_state)
    PowerLEDon();
}
