#include <stm32.h>
#include <string.h>
#include <usb_disk.h>
#include <usb_endianness.h>
#include <usbh_lib.h>
#include <usbh_msc_core.h>
#include <usbh_msc_req.h>
#include <usbh_std_req.h>

/** Very small subset of the SCSI command set **/

typedef struct {
  uint32_t last_lba;     /* big-endian format */
  uint32_t block_length; /* big-endian format */
} __packed scsi_capacity_data_t;

static uint32_t SCSITOHL(uint32_t x) {
  return __REV(x); /* Reverse endianness. */
}

static uint8_t const * SCSI_TEST_UNIT_READY6(void) {
  static const uint8_t cmd[6] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00
  };
  return cmd;
}

#define SCSI_SENSE_DATA_LEN  18

static uint8_t const * SCSI_REQUEST_SENSE6(uint8_t len) {
  static uint8_t cmd[6] = {
    0x03, 0x00, 0x00, 0x00, 0x00, 0x00
  };
  cmd[4] = len;
  return cmd;
}

#define SCSI_EJECT  2

static uint8_t const * SCSI_START_STOP_UNIT6(uint8_t action) {
  static uint8_t cmd[6] = {
    0x1B, 0x00, 0x00, 0x00, 0x00, 0x00
  };
  cmd[4] = action;
  return cmd;
}

static uint8_t const * SCSI_PREVENT_ALLOW_MEDIUM_REMOVAL6(int prevent) {
  static uint8_t cmd[6] = {
    0x1E, 0x00, 0x00, 0x00, 0x00, 0x00
  };
  cmd[4] = prevent != 0;
  return cmd;
}

static uint8_t const * SCSI_READ_CAPACITY10(void) {
  static const uint8_t cmd[10] = {
    0x25, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
  };
  return cmd;
}

static uint8_t const * SCSI_READ10(uint32_t lba, uint16_t lbc) {
  static uint8_t cmd[10] = {
    0x28, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
  };
  /* big-endian format */
  cmd[2] = lba >> 24;
  cmd[3] = lba >> 16;
  cmd[4] = lba >> 8;
  cmd[5] = lba;
  cmd[7] = lbc >> 8;
  cmd[8] = lbc;
  return cmd;
}

static uint8_t const * SCSI_WRITE10(uint32_t lba, uint16_t lbc) {
  static uint8_t cmd[10] = {
    0x2A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
  };
  /* big-endian format */
  cmd[2] = lba >> 24;
  cmd[3] = lba >> 16;
  cmd[4] = lba >> 8;
  cmd[5] = lba;
  cmd[7] = lbc >> 8;
  cmd[8] = lbc;
  return cmd;
}

/** Analyze descriptor content **/

static int CheckDeviceDescriptor(usb_device_descriptor_t *dev_desc) {
  if (dev_desc->bLength == sizeof(usb_device_descriptor_t) &&
      dev_desc->bDescriptorType == DEVICE_DESCRIPTOR &&
      dev_desc->bDeviceClass == 0 &&
      dev_desc->bDeviceSubClass == 0 &&
      dev_desc->bDeviceProtocol == 0) {
    return 0;
  }
  else {
    return -1;
  }
}

static int ParseConfiguration(msc_configuration_t *msc_cfg,
                              uint8_t const *buffer, uint16_t length) {
  usb_configuration_descriptor_t *cfg_desc;
  usb_interface_descriptor_t *if_desc;
  usb_endpoint_descriptor_t *ep_desc;
  int i;

  if (length < sizeof(usb_configuration_descriptor_t)) {
    return -1;
  }

  /* Prawidłowa kolejność bajtów składowej wTotalLength deskryptora
     konfiguracji jest ustalana w funkcji USBHgetConfDescriptor. */
  cfg_desc = (usb_configuration_descriptor_t *)buffer;
  if (cfg_desc->bLength != sizeof(usb_configuration_descriptor_t) ||
      cfg_desc->bDescriptorType != CONFIGURATION_DESCRIPTOR ||
      cfg_desc->wTotalLength != length) {
    return -1;
  }

  msc_cfg->configuration_value = cfg_desc->bConfigurationValue;

  buffer += cfg_desc->bLength;
  length -= cfg_desc->bLength;

  if (length < sizeof(usb_interface_descriptor_t)) {
    return -1;
  }

  if_desc = (usb_interface_descriptor_t *)buffer;
  if (if_desc->bLength != sizeof(usb_interface_descriptor_t) ||
      if_desc->bDescriptorType != INTERFACE_DESCRIPTOR) {
    return -1;
  }

  if (if_desc->bInterfaceClass != MASS_STORAGE_CLASS ||
      if_desc->bInterfaceSubClass != SCSI_TRANSPARENT_SUBCLASS ||
      if_desc->bInterfaceProtocol != BULK_ONLY_TRANSPORT_PROTOCOL) {
    return -1;
  }

  msc_cfg->interface_number = if_desc->bInterfaceNumber;

  buffer += if_desc->bLength;
  length -= if_desc->bLength;

  if (if_desc->bNumEndpoints != 2 ||
      length != 2 * sizeof(usb_endpoint_descriptor_t)) {
    return -1;
  }

  for (i = 0; i < 2; ++i) {
    ep_desc = (usb_endpoint_descriptor_t *)buffer;
    if (ep_desc->bLength != sizeof(usb_endpoint_descriptor_t) ||
        ep_desc->bDescriptorType != ENDPOINT_DESCRIPTOR ||
        ep_desc->bmAttributes != BULK_TRANSFER_BM) {
      return -1;
    }
    if ((ep_desc->bEndpointAddress & ENDP_DIRECTION_MASK) == ENDP_OUT) {
      msc_cfg->out_ep_addr = ep_desc->bEndpointAddress;
      msc_cfg->out_ep_max_packet = USBTOHS(ep_desc->wMaxPacketSize);
      /* Bulk transfers: the bInterval field is valid only for
         the out high speed endpoint. */
      if (msc_cfg->speed == HIGH_SPEED)
        msc_cfg->interval = ep_desc->bInterval;
      else
        msc_cfg->interval = 0;
    }
    else {
      msc_cfg->in_ep_addr = ep_desc->bEndpointAddress;
      msc_cfg->in_ep_max_packet = USBTOHS(ep_desc->wMaxPacketSize);
    }
    buffer += sizeof(usb_endpoint_descriptor_t);
  }

  if (msc_cfg->speed == FULL_SPEED &&
      msc_cfg->in_ep_max_packet <= MAX_FS_BULK_PACKET_SIZE &&
      msc_cfg->in_ep_max_packet >= 8) {
    return 0;
  }
  else if (msc_cfg->speed == HIGH_SPEED &&
           msc_cfg->in_ep_max_packet == MAX_HS_BULK_PACKET_SIZE) {
    return 0;
  }
  else {
    return -1;
  }
}

/** Disk interface **/

#define BUFF_LEN  256
#define TIMEOUT   1000000

static msc_configuration_t msc_cfg;

int USBdiskInitialize() {
  usb_device_descriptor_t *dev_desc;
  usb_configuration_descriptor_t *cfg_desc;
  scsi_capacity_data_t *disk_capacity;
  uint32_t length;
  uint16_t len16;
  uint8_t status, temp[BUFF_LEN];

  if (MSCisDeviceReady())
    return USBdiskStatus();

  memset(&temp, 0, BUFF_LEN);
  dev_desc = (usb_device_descriptor_t *)temp;
  if (USBHopenDevice(&msc_cfg.speed, &msc_cfg.dev_addr, dev_desc,
                     TIMEOUT) < 0)
    return -1;

  if (CheckDeviceDescriptor(dev_desc) < 0)
    return -1;

  memset(&temp, 0, BUFF_LEN);
  len16 = sizeof(usb_configuration_descriptor_t);
  if (USBHgetConfDescriptor(1, 0, temp, len16) < 0)
    return -1;

  cfg_desc = (usb_configuration_descriptor_t *)temp;
  len16 = USBTOHS(cfg_desc->wTotalLength);
  if (len16 > BUFF_LEN)
    return -1;

  memset(&temp, 0, BUFF_LEN);
  if (USBHgetConfDescriptor(1, 0, temp, len16) < 0)
    return -1;

  if (ParseConfiguration(&msc_cfg, temp, len16) < 0)
    return -1;

  if (USBHsetConfiguration(1, msc_cfg.configuration_value) < 0)
    return -1;

  if (MSCgetMaxLun(1, msc_cfg.interface_number, &msc_cfg.max_lun) < 0)
    return -1;

  if (MSCsetMachine(&msc_cfg) < 0)
    return -1;

  if (USBdiskStatus() < 0)
    return -1;

  length = sizeof(scsi_capacity_data_t);
  status = MSCBOT(1, 0, SCSI_READ_CAPACITY10(), 10, 0, temp, &length);
  if (status != MSC_BOT_CSW_COMMAND_PASSED)
    return -1;
  if (length != sizeof(scsi_capacity_data_t))
    return -1;
  disk_capacity = (scsi_capacity_data_t *)temp;
  length = SCSITOHL(disk_capacity->block_length);
  if (length > 0x1000)
    return -1;
  else
    msc_cfg.sector_size = length;

  return 0;
}

int USBdiskStatus() {
  int i;
  uint32_t length;
  uint8_t status, temp[SCSI_SENSE_DATA_LEN];

  for (i = 0; i < 2; ++i) {
    length = 0;
    status = MSCBOT(1, 0, SCSI_TEST_UNIT_READY6(), 6, 0, 0, &length);
    if (status == MSC_BOT_CSW_COMMAND_PASSED) {
      return 0;
    }
    else if (status == MSC_BOT_CSW_COMMAND_FAILED) {
      length = SCSI_SENSE_DATA_LEN;
      status = MSCBOT(1, 0, SCSI_REQUEST_SENSE6(SCSI_SENSE_DATA_LEN),
                      6, 0, temp, &length);
    }
    if (i == 0 && status == MSC_BOT_CSW_PHASE_ERROR) {
      if (MSCresetRecovery(1, msc_cfg.interface_number,
                           msc_cfg.in_ep_addr,
                           msc_cfg.out_ep_addr) < 0) {
        return -1;
      }
    }
    else if (status != MSC_BOT_CSW_COMMAND_PASSED &&
             status != MSC_BOT_CSW_COMMAND_FAILED) {
      return -1;
    }
  }
  return -1;
}

int USBdiskRead(uint8_t *buff, uint32_t sector, uint32_t count) {
  uint32_t length1, length2;
  uint8_t status;

  if (count > 0xFFFF)
    return -1;

  length1 = length2 = count * msc_cfg.sector_size;
  status = MSCBOT(1, 0, SCSI_READ10(sector, count), 10,
                  0, buff, &length1);

  return status != MSC_BOT_CSW_COMMAND_PASSED ||
         length1 != length2 ? -1 : 0;
}

int USBdiskWrite(uint8_t const *buff, uint32_t sector, uint32_t count) {
  uint32_t length1, length2;
  uint8_t status;

  if (count > 0xFFFF)
    return -1;

  length1 = length2 = count * msc_cfg.sector_size;
  status = MSCBOT(1, 0, SCSI_WRITE10(sector, count), 10,
                  buff, 0, &length1);

  return status != MSC_BOT_CSW_COMMAND_PASSED ||
         length1 != length2 ? -1 : 0;
}

int USBdiskSync() {
  return USBdiskStatus();
}

int USBdiskGetSectorSize(uint16_t *sector_size) {
  if (!MSCisDeviceReady())
    return -1;

  *sector_size = msc_cfg.sector_size;
  return 0;
}

int USBdiskAllowEject() {
  uint32_t length;
  uint8_t status1, status2;

  length = 0;
  status1 = MSCBOT(1, 0, SCSI_PREVENT_ALLOW_MEDIUM_REMOVAL6(0), 6,
                   0, 0, &length);
  length = 0;
  status2 = MSCBOT(1, 0, SCSI_START_STOP_UNIT6(SCSI_EJECT), 6,
                   0, 0, &length);

  return status1 != MSC_BOT_CSW_COMMAND_PASSED ||
         status2 != MSC_BOT_CSW_COMMAND_PASSED ? -1 : 0;
}
