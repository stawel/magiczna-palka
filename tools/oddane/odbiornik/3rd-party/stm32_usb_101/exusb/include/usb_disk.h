#ifndef _USB_DISK_H
#define _USB_DISK_H 1

#include <stdint.h>

#define USB_DRIVE_NUMEBR  0

int USBdiskInitialize(void);
int USBdiskStatus(void);
int USBdiskRead(uint8_t *, uint32_t, uint32_t);
int USBdiskWrite(uint8_t const *, uint32_t, uint32_t);
int USBdiskSync(void);
int USBdiskGetSectorSize(uint16_t *);
int USBdiskAllowEject(void);

#endif
