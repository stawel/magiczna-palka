#ifndef _BOARD_USB_DEF_H
#define _BOARD_USB_DEF_H 1

#include <xcat.h>

/** STM32L-Discovery board **/

#include <misc.h>
#include <stm32l1xx_gpio.h>
#include <stm32l1xx_rcc.h>
#include <stm32l1xx_syscfg.h>
#include <usb_def.h>

#define USB_BM_ATTRIBUTES  (D7_RESERVED)
#define USB_B_MAX_POWER    (20 / 2)

/* Definitions for usbd_configure_152.c */

/* Internal pullup resistor */
/* #define USB_PULLUP_DIRECT  0 */

/* External pullup resistor */
#define USB_PULLUP_DIRECT  1
#define USB_PULLUP_GPIO_N  C
#define USB_PULLUP_PIN_N   12

#endif
