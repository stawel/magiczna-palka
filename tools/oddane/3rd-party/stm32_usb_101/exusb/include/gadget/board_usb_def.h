#ifndef _BOARD_USB_DEF_H
#define _BOARD_USB_DEF_H 1

#include <xcat.h>

/** Gadget board **/

#include <misc.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_rcc.h>
#include <usb_def.h>

#define USB_BM_ATTRIBUTES  (D7_RESERVED)
#define USB_B_MAX_POWER    (400 / 2)

/* Definitions for usbd_configure_103.c */

#define USB_PULLUP_DIRECT  1
#define USB_PULLUP_ON      1
#define USB_PULLUP_GPIO_N  A
#define USB_PULLUP_PIN_N   10

#endif
