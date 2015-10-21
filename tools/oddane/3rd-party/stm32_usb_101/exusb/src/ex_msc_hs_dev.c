#include <auxiliary.h>
#include <board_usb_def.h>
#include <error.h>
#include <scsi.h>
#include <stm322xg_extsram.h>
#include <string.h>
#include <usb_endianness.h>
#include <usb_vid_pid.h>
#include <usbd_api.h>
#include <usbd_callbacks.h>

/* Switch logging on or off. */
#if 1
  #include <lcd_util.h>

  static void LOG(char const *msg) {
    LCDwriteWrap(msg);
  }
#else
  static void LOG(char const *msg, uint8_t const *cb, unsigned len) {
  }
#endif

/** Descriptors **/

static usb_device_descriptor_t device_descriptor = {
  sizeof(usb_device_descriptor_t), /* bLength */
  DEVICE_DESCRIPTOR,               /* bDescriptorType */
  HTOUSBS(0x0200),                 /* bcdUSB */
  0x00,                            /* bDeviceClass */
  0x00,                            /* bDeviceSubClass */
  0x00,                            /* bDeviceProtocol */
  0,                               /* bMaxPacketSize0 */
  HTOUSBS(VID),                    /* idVendor */
  HTOUSBS(PID + 6),                /* idProduct */
  HTOUSBS(0x0100),                 /* bcdDevice */
  1,                               /* iManufacturer */
  2,                               /* iProduct */
  3,                               /* iSerialNumber */
  1                                /* bNumConfigurations */
};

static usb_device_qualifier_descriptor_t device_qualifier = {
  sizeof(usb_device_qualifier_descriptor_t), /* bLength */
  DEVICE_QUALIFIER_DESCRIPTOR,               /* bDescriptorType */
  HTOUSBS(0x0200),                           /* bcdUSB */
  0x00,                                      /* bDeviceClass */
  0x00,                                      /* bDeviceSubClass */
  0x00,                                      /* bDeviceProtocol */
  0,                                         /* bMaxPacketSize0 */
  1,                                         /* bNumConfigurations */
  0                                          /* bReserved */
};

typedef struct {
  usb_configuration_descriptor_t cnf_descr;
  usb_interface_descriptor_t     if_descr;
  usb_endpoint_descriptor_t      ep_descr[2];
} __packed usb_configuration_t;

#ifndef USB_BM_ATTRIBUTES
  #define USB_BM_ATTRIBUTES  (SELF_POWERED | D7_RESERVED)
#endif
#ifndef USB_B_MAX_POWER
  #define USB_B_MAX_POWER  1
#endif

static usb_configuration_t msc_config = {
  {
    sizeof(usb_configuration_descriptor_t), /* bLength */
    CONFIGURATION_DESCRIPTOR,               /* bDescriptorType */
    HTOUSBS(sizeof(usb_configuration_t)),   /* wTotalLength */
    1,                                      /* bNumInterfaces */
    1,                                      /* bConfigurationValue */
    0,                                      /* iConfiguration */
    USB_BM_ATTRIBUTES,                      /* bmAttributes */
    USB_B_MAX_POWER                         /* bMaxPower */
  },
  {
    sizeof(usb_interface_descriptor_t),     /* bLength */
    INTERFACE_DESCRIPTOR,                   /* bDescriptorType */
    0,                                      /* bInterfaceNumber */
    0,                                      /* bAlternateSetting */
    2,                                      /* bNumEndpoints */
    MASS_STORAGE_CLASS,                     /* bInterfaceClass */
    SCSI_TRANSPARENT_SUBCLASS,              /* bInterfaceSubClass */
    BULK_ONLY_TRANSPORT_PROTOCOL,           /* bInterfaceProtocol */
    0                                       /* iInterface */
  },
  {
    {
      sizeof(usb_endpoint_descriptor_t),    /* bLength */
      ENDPOINT_DESCRIPTOR,                  /* bDescriptorType */
      ENDP1 | ENDP_IN,                      /* bEndpointAddress */
      BULK_TRANSFER,                        /* bmAttributes */
      0,                                    /* wMaxPacketSize */
      1                                     /* bInterval */
    },
    {
      sizeof(usb_endpoint_descriptor_t),    /* bLength */
      ENDPOINT_DESCRIPTOR,                  /* bDescriptorType */
      ENDP1 | ENDP_OUT,                     /* bEndpointAddress */
      BULK_TRANSFER,                        /* bmAttributes */
      0,                                    /* wMaxPacketSize */
      1                                     /* bInterval */
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

static usb_string_descriptor_t(44) const string_product = {
  sizeof(usb_string_descriptor_t(44)),
  STRING_DESCRIPTOR,
  {
    HTOUSBS('U'), HTOUSBS('S'), HTOUSBS('B'), HTOUSBS(' '),
    HTOUSBS('f'), HTOUSBS('u'), HTOUSBS('l'), HTOUSBS('l'),
    HTOUSBS(' '), HTOUSBS('a'), HTOUSBS('n'), HTOUSBS('d'),
    HTOUSBS(' '), HTOUSBS('h'), HTOUSBS('i'), HTOUSBS('g'),
    HTOUSBS('h'), HTOUSBS(' '), HTOUSBS('s'), HTOUSBS('p'),
    HTOUSBS('e'), HTOUSBS('e'), HTOUSBS('d'), HTOUSBS(' '),
    HTOUSBS('m'), HTOUSBS('a'), HTOUSBS('s'), HTOUSBS('s'),
    HTOUSBS(' '), HTOUSBS('s'), HTOUSBS('t'), HTOUSBS('o'),
    HTOUSBS('r'), HTOUSBS('a'), HTOUSBS('g'), HTOUSBS('e'),
    HTOUSBS(' '), HTOUSBS('e'), HTOUSBS('x'), HTOUSBS('a'),
    HTOUSBS('m'), HTOUSBS('p'), HTOUSBS('l'), HTOUSBS('e')
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

static usb_string_descriptor_t(10) const string_fs_config = {
  sizeof(usb_string_descriptor_t(10)),
  STRING_DESCRIPTOR,
  {
    HTOUSBS('F'), HTOUSBS('U'), HTOUSBS('L'), HTOUSBS('L'),
    HTOUSBS(' '), HTOUSBS('S'), HTOUSBS('P'), HTOUSBS('E'),
    HTOUSBS('E'), HTOUSBS('D')
  }
};

static usb_string_descriptor_t(10) const string_hs_config = {
  sizeof(usb_string_descriptor_t(10)),
  STRING_DESCRIPTOR,
  {
    HTOUSBS('H'), HTOUSBS('I'), HTOUSBS('G'), HTOUSBS('H'),
    HTOUSBS(' '), HTOUSBS('S'), HTOUSBS('P'), HTOUSBS('E'),
    HTOUSBS('E'), HTOUSBS('D')
  }
};

typedef struct {
  uint8_t const *data;
  uint16_t      length;
} string_table_t;

static string_table_t const strings[] = {
  {(uint8_t const*)&string_lang,         sizeof string_lang},
  {(uint8_t const*)&string_manufacturer, sizeof string_manufacturer},
  {(uint8_t const*)&string_product,      sizeof string_product},
  {(uint8_t const*)&string_serial,       sizeof string_serial},
  {(uint8_t const*)&string_fs_config,    sizeof string_fs_config},
  {(uint8_t const*)&string_hs_config,    sizeof string_hs_config}
};
static uint32_t const stringCount = sizeof(strings)/sizeof(strings[0]);

/** Application callbacks **/

static int          Configure(void);
static uint8_t      Reset(usb_speed_t);
static usb_result_t GetDescriptor(uint16_t, uint16_t,
                                  uint8_t const **, uint16_t *);
static uint8_t      GetConfiguration(void);
static usb_result_t SetConfiguration(uint16_t);
static uint16_t     GetStatus(void);
static usb_result_t ClassNoDataSetup(usb_setup_packet_t const *);
static usb_result_t ClassInDataSetup(usb_setup_packet_t const *,
                                     uint8_t const **, uint16_t *);
static void         EP1IN(void);
static void         EP1OUT(void);

static usbd_callback_list_t const ApplicationCallBacks = {
  Configure, Reset, 0, GetDescriptor, 0, GetConfiguration,
  SetConfiguration, GetStatus, 0, 0, 0, 0, ClassNoDataSetup,
  ClassInDataSetup, 0, 0,
  {EP1IN, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {EP1OUT, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  0, 0
};

/** Application initialization **/

/* Mass Storage Class, Bulk-Only Transport, state machine */
typedef enum {
  MSC_BOT_READY,
  MSC_BOT_DATA_OUT,
  MSC_BOT_DATA_IN,
  MSC_BOT_CSW,
  MSC_BOT_ERROR
} msc_bot_state_t;

static uint32_t        TransferSize;
static usb_speed_t     CurrentSpeed;
static msc_bot_state_t BOTstate;
static uint8_t         CurrentConfiguration;
static uint8_t         MaxLUN;

static void ResetState(void) {
  BOTstate = MSC_BOT_READY;
  CurrentConfiguration = 0;
  MaxLUN = SCSIgetMaxLUN();
}

usbd_callback_list_t const * USBDgetApplicationCallbacks() {
  return &ApplicationCallBacks;
}

int Configure() {
  SRAMconfigure();
  SCSIreset();
  ResetState();
  LOG("USB MSC SCSI RAM DISK\n");
  return 0;
}

uint8_t Reset(usb_speed_t speed) {
  ResetState();
  CurrentSpeed = speed;
  if (speed == FULL_SPEED) {
    device_descriptor.bMaxPacketSize0 = MAX_FS_CONTROL_PACKET_SIZE;
    device_qualifier.bMaxPacketSize0 = MAX_HS_CONTROL_PACKET_SIZE;
    msc_config.cnf_descr.iConfiguration = 4;
    TransferSize = MAX_FS_BULK_PACKET_SIZE;
    msc_config.ep_descr[0].wMaxPacketSize = HTOUSBS(TransferSize);
    msc_config.ep_descr[1].wMaxPacketSize = HTOUSBS(TransferSize);
    LOG("FS ");
  }
  else if (speed == HIGH_SPEED) {
    device_descriptor.bMaxPacketSize0 = MAX_HS_CONTROL_PACKET_SIZE;
    device_qualifier.bMaxPacketSize0 = MAX_FS_CONTROL_PACKET_SIZE;
    TransferSize = MAX_HS_BULK_PACKET_SIZE;
    msc_config.cnf_descr.iConfiguration = 5;
    msc_config.ep_descr[0].wMaxPacketSize = HTOUSBS(TransferSize);
    msc_config.ep_descr[1].wMaxPacketSize = HTOUSBS(TransferSize);
    LOG("HS ");
  }
  else
    ErrorResetable(-1, 6);

  /* Default control endpoint must be configured here. */
  if (USBDendPointConfigure(ENDP0, CONTROL_TRANSFER,
                            device_descriptor.bMaxPacketSize0,
                            device_descriptor.bMaxPacketSize0) !=
                                                      REQUEST_SUCCESS)
    ErrorResetable(-1, 7);

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
    case DEVICE_QUALIFIER_DESCRIPTOR:
      if (index == 0 && wIndex == 0) {
        *data = (uint8_t const *)&device_qualifier;
        *length = sizeof(usb_device_qualifier_descriptor_t);
        return REQUEST_SUCCESS;
      }
      return REQUEST_ERROR;
    case CONFIGURATION_DESCRIPTOR:
      if (index == 0 && wIndex == 0) {
        *data = (uint8_t const *)&msc_config;
        *length = sizeof(usb_configuration_t);
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
  return CurrentConfiguration;
}

usb_result_t SetConfiguration(uint16_t confValue) {
  if (confValue > device_descriptor.bNumConfigurations)
    return REQUEST_ERROR;

  CurrentConfiguration = confValue;
  USBDdisableAllNonControlEndPoints();
  if (confValue == msc_config.cnf_descr.bConfigurationValue)
    return USBDendPointConfigure(ENDP1, BULK_TRANSFER,
                                 TransferSize, TransferSize);

  return REQUEST_SUCCESS; /* confValue == 0 */
}

uint16_t GetStatus() {
  return STATUS_SELF_POWERED;
}

usb_result_t ClassNoDataSetup(usb_setup_packet_t const *setup) {
  if (setup->bmRequestType == (HOST_TO_DEVICE |
                               CLASS_REQUEST |
                               INTERFACE_RECIPIENT) &&
      setup->bRequest == MSC_BULK_ONLY_RESET &&
      setup->wValue == 0 &&
      setup->wIndex == 0 && /* interface number */
      setup->wLength == 0) {
    BOTstate = MSC_BOT_READY;
    SCSIreset();
    LOG("R ");
    return REQUEST_SUCCESS;
  }

  return REQUEST_ERROR;
}

usb_result_t ClassInDataSetup(usb_setup_packet_t const *setup,
                              uint8_t const **data,
                              uint16_t *length) {
  if (setup->bmRequestType == (DEVICE_TO_HOST |
                               CLASS_REQUEST |
                               INTERFACE_RECIPIENT) &&
      setup->bRequest == MSC_GET_MAX_LUN &&
      setup->wValue == 0 &&
      setup->wIndex == 0 && /* interface number */
      setup->wLength == 1) {
    *data = &MaxLUN;
    *length = 1;
    return REQUEST_SUCCESS;
  }

  return REQUEST_ERROR;
}

static int BOTreadCBW(msc_bot_cbw_t *p) {
  uint32_t len;

  len = USBDread(ENDP1, (uint8_t *)p, sizeof(msc_bot_cbw_t));
  if (len == sizeof(msc_bot_cbw_t)) {
    p->dCBWSignature          = USBTOHL(p->dCBWSignature);
    p->dCBWTag                = USBTOHL(p->dCBWTag);
    p->dCBWDataTransferLength = USBTOHL(p->dCBWDataTransferLength);
    return 0;
  }
  return -1;
}

static void BOTwriteCSW(msc_bot_csw_t *p) {
  p->dCSWSignature   = HTOUSBL(p->dCSWSignature);
  p->dCSWTag         = HTOUSBL(p->dCSWTag);
  p->dCSWDataResidue = HTOUSBL(p->dCSWDataResidue);

  USBDwrite(ENDP1, (const uint8_t *)p, sizeof(msc_bot_csw_t));
}

static msc_bot_cbw_t cbw;
static msc_bot_csw_t csw;
static uint8_t       *data;

void EP1OUT() {
  uint32_t len;
  scsi_direction_t transfer_direction;

  if (BOTstate == MSC_BOT_READY &&
      BOTreadCBW(&cbw) == 0 &&
      cbw.dCBWSignature == MSC_BOT_CBW_SIGNATURE &&
      (cbw.bmCBWFlags & MSC_BOT_CBW_FLAGS_RESERVED_BITS) == 0 &&
      cbw.bCBWLUN <= MaxLUN &&
      cbw.bCBWCBLength > 0 &&
      cbw.bCBWCBLength <= MSC_CBWCB_LENGTH) {
    csw.dCSWSignature = MSC_BOT_CSW_SIGNATURE;
    csw.dCSWTag = cbw.dCBWTag;
    csw.dCSWDataResidue = cbw.dCBWDataTransferLength;
    csw.bCSWStatus = SCSIcommand(cbw.bCBWLUN, cbw.CBWCB,
                                 cbw.bCBWCBLength, &data, &len,
                                 &transfer_direction);
    if (csw.bCSWStatus == MSC_BOT_CSW_COMMAND_FAILED ||
        (cbw.dCBWDataTransferLength == 0 && len == 0)) {
      BOTstate = MSC_BOT_CSW;
      BOTwriteCSW(&csw);
    }
    else if (cbw.dCBWDataTransferLength > 0 &&
             (cbw.dCBWDataTransferLength == len ||
              len < TransferSize) &&
             transfer_direction == SCSI_IN &&
             (cbw.bmCBWFlags & MSC_BOT_CBW_DATA_IN) != 0) {
      BOTstate = MSC_BOT_DATA_IN;
      cbw.dCBWDataTransferLength = len;
      len = min(cbw.dCBWDataTransferLength, TransferSize);
      len = USBDwrite(ENDP1, data, len);
      data += len;
      cbw.dCBWDataTransferLength -= len;
      csw.dCSWDataResidue -= len;
    }
    else if (cbw.dCBWDataTransferLength > 0 &&
             cbw.dCBWDataTransferLength == len &&
             transfer_direction == SCSI_OUT &&
             (cbw.bmCBWFlags & MSC_BOT_CBW_DATA_IN) == 0) {
      BOTstate = MSC_BOT_DATA_OUT;
    }
    else {
      BOTstate = MSC_BOT_ERROR;
      csw.bCSWStatus = MSC_BOT_CSW_PHASE_ERROR;
      BOTwriteCSW(&csw);
    }
  }
  else if (BOTstate == MSC_BOT_DATA_OUT) {
    len = min(cbw.dCBWDataTransferLength, TransferSize);
    len = USBDread(ENDP1, data, len);
    data += len;
    cbw.dCBWDataTransferLength -= len;
    csw.dCSWDataResidue -= len;
    if (cbw.dCBWDataTransferLength == 0) {
      BOTstate = MSC_BOT_CSW;
      BOTwriteCSW(&csw);
    }
  }
  else {
    USBDread(ENDP1, 0, 0); /* Empty receive buffer. */
    BOTstate = MSC_BOT_ERROR;
    USBDsetEndPointHalt(ENDP1 | ENDP_IN);
    USBDsetEndPointHalt(ENDP1 | ENDP_OUT);
    LOG("EO ");
  }
}

void EP1IN() {
  uint32_t len;

  if (BOTstate == MSC_BOT_DATA_IN) {
    if (cbw.dCBWDataTransferLength > 0) {
      len = min(cbw.dCBWDataTransferLength, TransferSize);
      len = USBDwrite(ENDP1, data, len);
      data += len;
      cbw.dCBWDataTransferLength -= len;
      csw.dCSWDataResidue -= len;
    }
    else {
      BOTstate = MSC_BOT_CSW;
      BOTwriteCSW(&csw);
    }
  }
  else if (BOTstate == MSC_BOT_CSW) {
    BOTstate = MSC_BOT_READY;
  }
  else {
    BOTstate = MSC_BOT_ERROR;
    USBDsetEndPointHalt(ENDP1 | ENDP_IN);
    USBDsetEndPointHalt(ENDP1 | ENDP_OUT);
    LOG("EI ");
  }
}
