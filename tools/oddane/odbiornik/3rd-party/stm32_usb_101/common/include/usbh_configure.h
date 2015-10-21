#ifndef _USBH_CONFIGURE_H
#define _USBH_CONFIGURE_H 1

#include <usb_def.h>

/** USB host initialization API **/

int USBHconfigure(usb_phy_t);
void USBHvbus(int);

#endif
