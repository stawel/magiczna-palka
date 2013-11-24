#ifndef _BOARD_USB_DEF_H
#define _BOARD_USB_DEF_H 1

#include <xcat.h>

/** ZL29ARM board **/

#include <misc.h>
#include <stm32f10x_exti.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_pwr.h>
#include <stm32f10x_rcc.h>
#include <usb_def.h>

#define USB_BM_ATTRIBUTES  (SELF_POWERED | D7_RESERVED)
#define USB_B_MAX_POWER    (2 / 2)

/* Definitions for usbh_configure_107.c and usbh_interrupt.c */

#define HOST_VBUS_GPIO_N  D
#define HOST_VBUS_PIN_N   15
#define HOST_VBUS_ON      1

#define HOST_OVRCURR_GPIO_N  D
#define HOST_OVRCURR_PIN_N   14
#define HOST_OVRCURR_IRQ_N   EXTI15_10
#define HOST_OVRCURR_EDGE    EXTI_Trigger_Falling

#endif
