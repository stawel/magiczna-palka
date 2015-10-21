#ifndef _BOARD_USB_DEF_H
#define _BOARD_USB_DEF_H 1

#include <xcat.h>

/** STM32F4-Discovery board **/

#include <misc.h>
#include <stm32f4xx_exti.h>
#include <stm32f4xx_gpio.h>
#include <stm32f4xx_pwr.h>
#include <stm32f4xx_rcc.h>
#include <stm32f4xx_syscfg.h>
#include <usb_def.h>

#define USB_BM_ATTRIBUTES  (SELF_POWERED | D7_RESERVED)
#define USB_B_MAX_POWER    (2 / 2)

#endif
