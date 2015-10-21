#ifndef _BOARD_DEF_H
#define _BOARD_DEF_H 1

//#include <misc.h>
//#include <stm32l1xx_exti.h>
#include <stm32l1xx_flash.h>
#include <stm32l1xx_gpio.h>
#include <stm32l1xx_lcd.h>
#include <stm32l1xx_pwr.h>
#include <stm32l1xx_rcc.h>
//#include <stm32l1xx_syscfg.h>
//#include <stm32l1xx_tim.h>
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
#if HSE_VALUE != 8000000 && HSE_VALUE != 0
  #error STM32L-Discovery board board uses 8 MHz HSE or 16 MHz HSI.
#endif

/* Mikrokontroler jest taktowany przez ST-LINK/V2. */
/* #define HSE_BYPASS  1 */

/* Definitions for led.c */

#define GREEN_LED_GPIO_N  B
#define GREEN_LED_PIN_N   7

#define RED_LED_GPIO_N    B
#define RED_LED_PIN_N     6

#define LED_ON            1

#endif
