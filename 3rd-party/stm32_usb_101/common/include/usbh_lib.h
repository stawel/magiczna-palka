#ifndef _USBH_LIB_H
#define _USBH_LIB_H 1

#include <usb_def.h>

int USBHopenDevice(usb_speed_t *, uint8_t *,
                   usb_device_descriptor_t *, int);
int USBHisDeviceReady(void);
void USBHdeviceHardReset(unsigned);

#endif
