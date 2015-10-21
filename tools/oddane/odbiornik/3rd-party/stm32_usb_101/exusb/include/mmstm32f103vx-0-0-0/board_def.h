#ifndef _BOARD_DEF_H
#define _BOARD_DEF_H 1

#include <misc.h>
#include <stm32f10x_exti.h>
#include <stm32f10x_flash.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_tim.h>
#include <xcat.h>

/** Board with MMSTM32F103Vx-0-0-0 module **/

/* Wszystkie stałe powinny być definiowane w plikach nagłówkowych,
   ale Standard Peripheral Library na to nie pozwala... */
#ifndef USE_STDPERIPH_DRIVER
  #error USE_STDPERIPH_DRIVER not defined
#endif
#ifndef STM32F10X_HD
  #error STM32F10X_HD not defined
#endif
#ifndef HSE_VALUE
  #error HSE_VALUE not defined
#endif

/* Zapewnij spójność stałych definiowanych w różnych miejscach. */
#if HSE_VALUE != 8000000
  #error MMSTM32F103Vx-0-0-0 board uses 8 MHz external quarz.
#endif

/* Definitions for boot_10x.c */

#define BOOT1_GPIO_N  A
#define BOOT1_PIN_N   4

/* Definitions for led.c */

#define RED_LED_GPIO_N    E
#define GREEN_LED_GPIO_N  E
#define RED_LED_PIN_N     11
#define GREEN_LED_PIN_N   12
#define LED_ON            0

/* Definitions for timer.c */

#define MS_TIM_N  3

/* Definitions for ext_periph_10x.c */

#define POWER_LED_GPIO_N  A
#define POWER_LED_PIN_N   0
#define POWER_LED_ON      1

#define PUSH_BUTTON_GPIO_N  A
#define PUSH_BUTTON_PIN_N   1
#define PUSH_BUTTON_IRQ_N   EXTI1
#define PUSH_BUTTON_EDGE    EXTI_Trigger_Falling

#endif
