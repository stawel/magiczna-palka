#include <board_usb_def.h>
#include <dac.h>
#include <error.h>
#include <lcd_util.h>
#include <stdio.h>
#include <usb_endianness.h>
#include <usb_vid_pid.h>
#include <usbd_api.h>
#include <usbd_callbacks.h>

/* NOTE: STM32F107 limitation: samples are lost if clock is 48 MHz. */

/** Descriptors **/

/* 96 bytes per frame are needed for one channel with sampling
   frequency 48 kHz and 16-bit resolution. */
#define ISO_BUFF_SIZE  96

static usb_device_descriptor_t const device_descriptor = {
  sizeof(usb_device_descriptor_t), /* bLength */
  DEVICE_DESCRIPTOR,               /* bDescriptorType */
  HTOUSBS(0x0200),                 /* bcdUSB */
  0,                               /* bDeviceClass */
  0,                               /* bDeviceSubClass */
  0,                               /* bDeviceProtocol */
  64,                              /* bMaxPacketSize0 */
  HTOUSBS(VID),                    /* idVendor */
  HTOUSBS(PID + 3),                /* idProduct */
  HTOUSBS(0x0100),                 /* bcdDevice */
  1,                               /* iManufacturer */
  2,                               /* iProduct */
  3,                               /* iSerialNumber */
  1                                /* bNumConfigurations */
};

typedef struct {
  usb_configuration_descriptor_t           cnf_descr;
  usb_interface_descriptor_t               if0_descr;
  usb_ac_header_descriptor_t               ac_h_descr;
  usb_ac_input_terminal_descriptor_t       ac_it_descr;
  usb_ac_feature_unit_descriptor_t         ac_fu_descr;
  usb_ac_output_terminal_descriptor_t      ac_ot_descr;
  usb_interface_descriptor_t               if10_descr;
  usb_interface_descriptor_t               if11_descr;
  usb_as_general_descriptor_t              as_g_descr;
  usb_as_format_type_descriptor_t          as_ft_descr;
  usb_std_audio_data_endpoint_descriptor_t std_ep1in_descr;
  usb_cs_audio_data_endpoint_descriptor_t  cs_ep1in_descr;
} __packed usb_audio_configuration_t;

#ifndef USB_BM_ATTRIBUTES
  #define USB_BM_ATTRIBUTES  (SELF_POWERED | D7_RESERVED)
#endif
#ifndef USB_B_MAX_POWER
  #define USB_B_MAX_POWER  1
#endif

static usb_audio_configuration_t const audio_configuration = {
  {
    sizeof(usb_configuration_descriptor_t),     /* bLength */
    CONFIGURATION_DESCRIPTOR,                   /* bDescriptorType */
    HTOUSBS(sizeof(usb_audio_configuration_t)), /* wTotalLength */
    2,                                          /* bNumInterfaces */
    1,                                       /* bConfigurationValue */
    0,                                       /* iConfiguration */
    USB_BM_ATTRIBUTES,                       /* bmAttributes */
    USB_B_MAX_POWER                          /* bMaxPower */
  },
  {
    sizeof(usb_interface_descriptor_t), /* bLength */
    INTERFACE_DESCRIPTOR,               /* bDescriptorType */
    0,                                  /* bInterfaceNumber */
    0,                                  /* bAlternateSetting */
    0,                                  /* bNumEndpoints */
    AUDIO_CLASS,                        /* bInterfaceClass */
    AUDIOCONTROL_SUBCLASS,              /* bInterfaceSubClass */
    0,                                  /* bInterfaceProtocol */
    0                                   /* iInterface */
  },
  {
    sizeof(usb_ac_header_descriptor_t), /* bLength */
    CS_INTERFACE_DESCRIPTOR,            /* bDescriptorType */
    AC_HEADER_DESCRIPTOR,               /* bDescriptorSubtype */
    HTOUSBS(0x100),                     /* bcdADC */
    HTOUSBS(sizeof(usb_ac_header_descriptor_t) +
            sizeof(usb_ac_input_terminal_descriptor_t) +
            sizeof(usb_ac_feature_unit_descriptor_t) +
            sizeof(usb_ac_output_terminal_descriptor_t)),
                                        /* wTotalLength */
    1,                                  /* bInCollection */
    1                                   /* baInterfaceNr */
  },
  {
    sizeof(usb_ac_input_terminal_descriptor_t), /* bLength */
    CS_INTERFACE_DESCRIPTOR,                    /* bDescriptorType */
    AC_INPUT_TERMINAL_DESCRIPTOR,             /* bDescriptorSubtype */
    1,                                        /* bTerminalID */
    HTOUSBS(0x0101), /* audio streaming */    /* wTerminalType */
    0,                                        /* bAssocTerminal */
    1,                                        /* bNrChannels */
    HTOUSBS(0x0004), /* center front */       /* wChannelConfig */
    0,                                        /* iChannelNames */
    0                                         /* iTerminal */
  },
  {
    sizeof(usb_ac_feature_unit_descriptor_t), /* bLength */
    CS_INTERFACE_DESCRIPTOR,                  /* bDescriptorType */
    AC_FEATURE_UNIT_DESCRIPTOR,               /* bDescriptorSubtype */
    2,                                        /* bUnitID */
    1,                                        /* bSourceID */
    2,                                        /* bControlSize */
    HTOUSBS(0x0003), /* mute & volume */      /* bmaControls0 */
    0                                         /* iFeature */
  },
  {
    sizeof(usb_ac_output_terminal_descriptor_t), /* bLength */
    CS_INTERFACE_DESCRIPTOR,                     /* bDescriptorType */
    AC_OUTPUT_TERMINAL_DESCRIPTOR,            /* bDescriptorSubtype */
    3,                                        /* bTerminalID */
    HTOUSBS(0x0304), /* desktop speaker */    /* wTerminalType */
    0,                                        /* bAssocTerminal */
    2,                                        /* bSourceID */
    0                                         /* iTerminal */
  },
  {
    sizeof(usb_interface_descriptor_t), /* bLength */
    INTERFACE_DESCRIPTOR,               /* bDescriptorType */
    1,                                  /* bInterfaceNumber */
    0,                                  /* bAlternateSetting */
    0,                                  /* bNumEndpoints */
    AUDIO_CLASS,                        /* bInterfaceClass */
    AUDIOSTREAMING_SUBCLASS,            /* bInterfaceSubClass */
    0,                                  /* bInterfaceProtocol */
    0                                   /* iInterface */
  },
  {
    sizeof(usb_interface_descriptor_t), /* bLength */
    INTERFACE_DESCRIPTOR,               /* bDescriptorType */
    1,                                  /* bInterfaceNumber */
    1,                                  /* bAlternateSetting */
    1,                                  /* bNumEndpoints */
    AUDIO_CLASS,                        /* bInterfaceClass */
    AUDIOSTREAMING_SUBCLASS,            /* bInterfaceSubClass */
    0,                                  /* bInterfaceProtocol */
    0                                   /* iInterface */
  },
  {
    sizeof(usb_as_general_descriptor_t), /* bLength */
    CS_INTERFACE_DESCRIPTOR,             /* bDescriptorType */
    AS_GENERAL_DESCRIPTOR,               /* bDescriptorSubtype */
    1,                                   /* bTerminalLink */
    2, /* in ms frames */                /* bDelay */
    HTOUSBS(0x0001) /* PCM */            /* wFormatTag */
  },
  {
    sizeof(usb_as_format_type_descriptor_t), /* bLength */
    CS_INTERFACE_DESCRIPTOR,                 /* bDescriptorType */
    AS_FORMAT_TYPE_DESCRIPTOR,               /* bDescriptorSubtype */
    1,                                       /* bFormatType */
    1,                                       /* bNrChannels */
    2, /* 2 bytes per audio subframe */      /* bSubFrameSize */
    16,                                      /* bBitResolution */
    1, /* one frequency supported */         /* bSamFreqType */
    {0x80, 0xbb, 0x00} /* 48 kHz */          /* tSamFreq */
  },
  {
    sizeof(usb_std_audio_data_endpoint_descriptor_t), /* bLength */
    ENDPOINT_DESCRIPTOR,                        /* bDescriptorType */
    ENDP1 | ENDP_OUT,                           /* bEndpointAddress */
    ISOCHRONOUS_TRANSFER | ADAPTIVE_ENDPOINT,   /* bmAttributes */
    HTOUSBS(ISO_BUFF_SIZE),                     /* wMaxPacketSize */
    1,                                          /* bInterval */
    0,                                          /* bRefresh */
    0                                           /* bSynchAddress */
  },
  {
    sizeof(usb_cs_audio_data_endpoint_descriptor_t), /* bLength */
    CS_ENDPOINT_DESCRIPTOR, /* bDescriptorType */
    EP_GENERAL_DESCRIPTOR,  /* bDescriptorSubtype */
    0,                      /* bmAttributes */
    2, /* in samples */     /* bLockDelayUnits */
    HTOUSBS(2400)           /* wLockDelay */
  },
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

static usb_string_descriptor_t(19) const string_product = {
  sizeof(usb_string_descriptor_t(19)),
  STRING_DESCRIPTOR,
  {
    HTOUSBS('U'), HTOUSBS('S'), HTOUSBS('B'), HTOUSBS(' '),
    HTOUSBS('s'), HTOUSBS('p'), HTOUSBS('e'), HTOUSBS('a'),
    HTOUSBS('k'), HTOUSBS('e'), HTOUSBS('r'), HTOUSBS(' '),
    HTOUSBS('e'), HTOUSBS('x'), HTOUSBS('a'), HTOUSBS('m'),
    HTOUSBS('p'), HTOUSBS('l'), HTOUSBS('e')
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

typedef struct {
  uint8_t const *data;
  uint16_t      length;
} string_table_t;

static string_table_t const strings[] = {
  {(uint8_t const*)&string_lang,         sizeof string_lang},
  {(uint8_t const*)&string_manufacturer, sizeof string_manufacturer},
  {(uint8_t const*)&string_product,      sizeof string_product},
  {(uint8_t const*)&string_serial,       sizeof string_serial}
};
static uint32_t const stringCount = sizeof(strings)/sizeof(strings[0]);

/** Callbacks **/

static int          Configure(void);
static uint8_t      Reset(usb_speed_t);
static usb_result_t GetDescriptor(uint16_t, uint16_t,
                                  uint8_t const **, uint16_t *);
static uint8_t      GetConfiguration(void);
static usb_result_t SetConfiguration(uint16_t);
static usb_result_t GetInterface(uint16_t, uint8_t *);
static usb_result_t SetInterface(uint16_t, uint16_t);
static uint16_t     GetStatus(void);
static usb_result_t ClassInDataSetup(usb_setup_packet_t const *,
                                     uint8_t const **, uint16_t *);
static usb_result_t ClassOutDataSetup(usb_setup_packet_t const *,
                                      uint8_t **);
static void         ClassStatusIn(usb_setup_packet_t const *);
static void         EP1OUT(void);

static usbd_callback_list_t const ApplicationCallBacks = {
  Configure, Reset, 0, GetDescriptor, 0, GetConfiguration,
  SetConfiguration, GetStatus, GetInterface, SetInterface, 0, 0, 0,
  ClassInDataSetup, ClassOutDataSetup, ClassStatusIn,
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {EP1OUT, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  0, 0
};

/** Speaker implementation **/

static unsigned progress;
static uint8_t  configuration, interface1, refresh;

#define LCD_REFRESH  8

static void LCDrefresh(void) {
  static const char fan[4] = {'|', '/', '-', '\\'};
  char buffer[20];

  if (refresh) {
    refresh = 0;
    sprintf(buffer, "%3hd dB %s %c",
            DACgetVolume() >> 8, DACgetMute() ? "MUTE" : "    ",
            interface1 ? fan[(progress >> LCD_REFRESH) & 3] : ' ');
    LCDgoto(0, 0);
    LCDwrite(buffer);
  }
}

static void ResetState(void) {
  DACreset();
  progress = 0;
  configuration = 0;
  interface1 = 0;
  refresh = 1;
}

usbd_callback_list_t const * USBDgetApplicationCallbacks() {
  return &ApplicationCallBacks;
}

int Configure() {
  ResetState();
  LCDsetRefresh(LCDrefresh);
  return DACconfigure(48000);
}

uint8_t Reset(usb_speed_t speed) {
  ErrorResetable(speed == FULL_SPEED ? 0 : -1, 6);
  ResetState();

  /* Default control endpoint must be configured here. */
  if (USBDendPointConfigure(ENDP0, CONTROL_TRANSFER,
                            device_descriptor.bMaxPacketSize0,
                            device_descriptor.bMaxPacketSize0) !=
                                                      REQUEST_SUCCESS)
    ErrorResetable(-1, 7);

  return device_descriptor.bMaxPacketSize0;
}

usb_result_t GetDescriptor(uint16_t wValue, uint16_t wIndex,
                           uint8_t const **data, uint16_t *length) {
  uint32_t index = wValue & 0xff;

  switch (wValue >> 8) {
    case DEVICE_DESCRIPTOR:
      if (index == 0 && wIndex == 0) {
        *data = (uint8_t const *)&device_descriptor;
        *length = sizeof(device_descriptor);
        return REQUEST_SUCCESS;
      }
      return REQUEST_ERROR;
    case CONFIGURATION_DESCRIPTOR:
      if (index == 0 && wIndex == 0) {
        *data = (uint8_t const *)&audio_configuration;
        *length = sizeof(audio_configuration);
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
  if (confValue > device_descriptor.bNumConfigurations)
    return REQUEST_ERROR;

  configuration = confValue;
  USBDdisableAllNonControlEndPoints();
  if (confValue == audio_configuration.cnf_descr.bConfigurationValue)
    return USBDendPointConfigure(ENDP1, ISOCHRONOUS_TRANSFER,
                                 ISO_BUFF_SIZE, 0);

  return REQUEST_SUCCESS; /* confValue == 0 */
}

usb_result_t GetInterface(uint16_t interface, uint8_t *setting) {
  if (interface == 0) {
    *setting = 0;
    return REQUEST_SUCCESS;
  }
  else if (interface == 1) {
    *setting = interface1;
    return REQUEST_SUCCESS;
  }
  else {
    *setting = 0;
    return REQUEST_ERROR;
  }
}

usb_result_t SetInterface(uint16_t interface, uint16_t setting) {
  if (interface == 0 && setting == 0)
    return REQUEST_SUCCESS;
  else if (interface == 1 && setting <= 1) {
    interface1 = setting;
    refresh = 1;
    return REQUEST_SUCCESS;
  }
  else
    return REQUEST_ERROR;
}

uint16_t GetStatus() {
  /* Current power setting should be reported. */
  if (audio_configuration.cnf_descr.bmAttributes & SELF_POWERED)
    return STATUS_SELF_POWERED;
  else
    return 0;
}

static uint8_t  buffer8;
static uint16_t buffer16;

usb_result_t ClassInDataSetup(usb_setup_packet_t const *setup,
                              uint8_t const **data,
                              uint16_t *length) {
  if (setup->bmRequestType == (DEVICE_TO_HOST |
                               CLASS_REQUEST |
                               INTERFACE_RECIPIENT)) {
    if (setup->bRequest == GET_CUR &&
        setup->wValue == MUTE_CONTROL << 8 &&
        setup->wIndex == 0x0200 &&
        setup->wLength == 1) {
      buffer8 = DACgetMute();
      *data = &buffer8;
      *length = 1;
      return REQUEST_SUCCESS;
    }
    else if (setup->bRequest == GET_CUR &&
             setup->wValue == VOLUME_CONTROL << 8 &&
             setup->wIndex == 0x0200 &&
             setup->wLength == 2) {
      buffer16 = HTOUSBS(DACgetVolume());
      *data = (uint8_t const *)&buffer16;
      *length = 2;
      return REQUEST_SUCCESS;
    }
    else if (setup->bRequest == GET_MIN &&
             setup->wValue == VOLUME_CONTROL << 8 &&
             setup->wIndex == 0x0200 &&
             setup->wLength == 2) {
      buffer16 = HTOUSBS(DACgetVolumeMin());
      *data = (uint8_t const *)&buffer16;
      *length = 2;
      return REQUEST_SUCCESS;
    }
    else if (setup->bRequest == GET_MAX &&
             setup->wValue == VOLUME_CONTROL << 8 &&
             setup->wIndex == 0x0200 &&
             setup->wLength == 2) {
      buffer16 = HTOUSBS(DACgetVolumeMax());
      *data = (uint8_t const *)&buffer16;
      *length = 2;
      return REQUEST_SUCCESS;
    }
    else if (setup->bRequest == GET_RES &&
             setup->wValue == VOLUME_CONTROL << 8 &&
             setup->wIndex == 0x0200 &&
             setup->wLength == 2) {
      buffer16 = HTOUSBS(DACgetResolution());
      *data = (uint8_t const *)&buffer16;
      *length = 2;
      return REQUEST_SUCCESS;
    }
  }
  return REQUEST_ERROR;
}

usb_result_t ClassOutDataSetup(usb_setup_packet_t const *setup,
                               uint8_t **data) {
  if (setup->bmRequestType == (HOST_TO_DEVICE |
                               CLASS_REQUEST |
                               INTERFACE_RECIPIENT)) {
    if (setup->bRequest == SET_CUR &&
        setup->wValue == MUTE_CONTROL << 8 &&
        setup->wIndex == 0x0200 &&
        setup->wLength == 1) {
      *data = &buffer8;
      return REQUEST_SUCCESS;
    }
    else if (setup->bRequest == SET_CUR &&
             setup->wValue == VOLUME_CONTROL << 8 &&
             setup->wIndex == 0x0200 &&
             setup->wLength == 2) {
      *data = (uint8_t *)&buffer16;
      return REQUEST_SUCCESS;
    }
    else if (setup->bRequest == SET_RES &&
             setup->wValue == VOLUME_CONTROL << 8 &&
             setup->wIndex == 0x0200 &&
             setup->wLength == 2) {
      *data = (uint8_t *)&buffer16;
      return REQUEST_SUCCESS;
    }
  }
  return REQUEST_ERROR;
}

void ClassStatusIn(usb_setup_packet_t const *setup) {
  if (setup->bmRequestType == (HOST_TO_DEVICE |
                               CLASS_REQUEST |
                               INTERFACE_RECIPIENT)) {
    if (setup->bRequest == SET_CUR &&
        setup->wValue == MUTE_CONTROL << 8 &&
        setup->wIndex == 0x0200 &&
        setup->wLength == 1) {
      DACsetMute(buffer8);
      refresh = 1;
    }
    else if (setup->bRequest == SET_CUR &&
        setup->wValue == VOLUME_CONTROL << 8 &&
        setup->wIndex == 0x0200 &&
        setup->wLength == 2) {
      DACsetVolume(USBTOHS(buffer16));
      refresh = 1;
    }
    else if (setup->bRequest == SET_RES &&
             setup->wValue == VOLUME_CONTROL << 8 &&
             setup->wIndex == 0x0200 &&
             setup->wLength == 2) {
      DACsetResolution(USBTOHS(buffer16));
      refresh = 1;
    }
  }
}

void EP1OUT() {
  int16_t  buffer[(ISO_BUFF_SIZE + 1) >> 1];
  uint32_t received, i;

  received = USBDread(ENDP1, (uint8_t *)buffer, ISO_BUFF_SIZE);
  received >>= 1;
  /* Loop is optimized when host is little-endian. */
  for (i = 0; i < received; ++i)
    buffer[i] = USBTOHS(buffer[i]);
  DACputSamples(buffer, received);
  ++progress;
  if ((progress & ((1U << LCD_REFRESH) - 1)) == 0)
    refresh = 1;
}
