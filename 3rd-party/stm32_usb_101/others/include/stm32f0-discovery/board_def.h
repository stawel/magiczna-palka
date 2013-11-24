#ifndef _BOARD_DEF_H
#define _BOARD_DEF_H 1

#include <stm32f0xx_flash.h>
#include <stm32f0xx_gpio.h>
#include <stm32f0xx_rcc.h>
#include <xcat.h>

/** STM32F0-Discovery board **/

/* Wszystkie stałe powinny być definiowane w plikach nagłówkowych,
   ale Standard Peripheral Library na to nie pozwala... */
#ifndef USE_STDPERIPH_DRIVER
  #error USE_STDPERIPH_DRIVER not defined
#endif
#ifndef STM32F0XX
  #error STM32F0XX not defined
#endif
#ifndef HSE_VALUE
  #error HSE_VALUE not defined
#endif
#if HSE_VALUE == 8000000
  #define HSE_BYPASS  1
#elif HSE_VALUE != 0
  #error Wrong clock for STM32F0-Discovery board.
#endif

/* Definitions for led.c
   Green LED is LD3 (green) and red LED is LD4 (blue). */

#define GREEN_LED_GPIO_N  C
#define GREEN_LED_PIN_N   9

#define RED_LED_GPIO_N    C
#define RED_LED_PIN_N     8

#define LED_ON            1

#endif
