#ifndef _BOARD_DEF_H
#define _BOARD_DEF_H 1

#include <misc.h>
#include <stm32l1xx_exti.h>
#include <stm32l1xx_flash.h>
#include <stm32l1xx_gpio.h>
#include <stm32l1xx_lcd.h>
#include <stm32l1xx_pwr.h>
#include <stm32l1xx_rcc.h>
#include <stm32l1xx_syscfg.h>
#include <stm32l1xx_tim.h>
#include <xcat.h>

/** STM32L-Discovery board **/

/* Wszystkie stałe powinny być definiowane w plikach nagłówkowych,
   ale Standard Peripheral Library na to nie pozwala... */
#ifndef USE_STDPERIPH_DRIVER
  #error USE_STDPERIPH_DRIVER not defined
#endif
#ifndef STM32L1XX_MD
  #error STM32L1XX_MD not defined
#endif
#ifndef HSE_VALUE
  #error HSE_VALUE not defined
#endif

/* Zapewnij spójność stałych definiowanych w różnych miejscach. */
#if HSE_VALUE != 8000000
  #error STM32L-Discovery board board uses 8 MHz external quarz.
#endif

/* Definitions for led.c */

#define GREEN_LED_GPIO_N  B
#define GREEN_LED_PIN_N   7

#define RED_LED_GPIO_N    A
#define RED_LED_PIN_N     5

#define LED_ON            1

/* Definitions for timer.c */

#define MS_TIM_N  3

/* Definitions for ext_periph_15x.c */

#define POWER_LED_GPIO_N  B
#define POWER_LED_PIN_N   6
#define POWER_LED_ON      1

#define PUSH_BUTTON_GPIO_N  A
#define PUSH_BUTTON_PIN_N   0
#define PUSH_BUTTON_IRQ_N   EXTI0
#define PUSH_BUTTON_EDGE    EXTI_Trigger_Rising

#endif
