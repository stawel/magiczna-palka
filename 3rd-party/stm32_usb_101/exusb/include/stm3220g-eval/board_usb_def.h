#ifndef _BOARD_USB_DEF_H
#define _BOARD_USB_DEF_H 1

#include <xcat.h>

/** STM3220G-EVAL board **/

#include <misc.h>
#include <stm32f2xx_exti.h>
#include <stm32f2xx_gpio.h>
#include <stm32f2xx_pwr.h>
#include <stm32f2xx_rcc.h>
#include <stm32f2xx_syscfg.h>
#include <usb_def.h>

#define USB_BM_ATTRIBUTES  (SELF_POWERED | D7_RESERVED)
#define USB_B_MAX_POWER    (2 / 2)

/* Definitions for usbh_configure_207.c and usbh_interrupt.c */

#define HOST_VBUS_GPIO_N  H
#define HOST_VBUS_PIN_N   5
#define HOST_VBUS_ON      0

#define HOST_OVRCURR_GPIO_N  F
#define HOST_OVRCURR_PIN_N   11
#define HOST_OVRCURR_IRQ_N   EXTI15_10
#define HOST_OVRCURR_EDGE    EXTI_Trigger_Falling

#endif
