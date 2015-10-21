#ifndef _BOARD_DEF_H
#define _BOARD_DEF_H 1

#include <misc.h>
#include <stm32f4xx_dma.h>
#include <stm32f4xx_exti.h>
#include <stm32f4xx_flash.h>
#include <stm32f4xx_gpio.h>
#include <stm32f4xx_i2c.h>
#include <stm32f4xx_rcc.h>
#include <stm32f4xx_spi.h>
#include <stm32f4xx_syscfg.h>
#include <stm32f4xx_tim.h>
#include <xcat.h>

/** STM32F4-Discovery board **/

/* Wszystkie stałe powinny być definiowane w plikach nagłówkowych,
   ale Standard Peripheral Library na to nie pozwala... */
#ifndef USE_STDPERIPH_DRIVER
  #error USE_STDPERIPH_DRIVER not defined
#endif
#ifndef STM32F4XX
  #error STM32F4XX not defined
#endif
#ifndef HSE_VALUE
  #error HSE_VALUE not defined
#endif

/* Zapewnij spójność stałych definiowanych w różnych miejscach. */
#if HSE_VALUE != 8000000
  #error STM32F4-Discovery board uses 8 MHz external quarz.
#endif

/* Definitions for i2c.c */

#define I2C_N          1
#define I2C_GPIO_N     B
#define I2C_SCL_PIN_N  6
#define I2C_SDA_PIN_N  9

/* Definitions for i2s.c */

#define I2S_N           3
#define I2S_MCK_GPIO_N  C
#define I2S_MCK_PIN_N   7
#define I2S_SCK_GPIO_N  C
#define I2S_SCK_PIN_N   10
#define I2S_SD_GPIO_N   C
#define I2S_SD_PIN_N    12
#define I2S_WS_GPIO_N   A
#define I2S_WS_PIN_N    4

#define I2S_DMA_N          1
#define I2S_DMA_CHANNEL_N  0
#define I2S_DMA_STREAM_N   7

/* Definitions for led.c */

#define GREEN_LED_GPIO_N  D
#define GREEN_LED_PIN_N   12

#define RED_LED_GPIO_N    D
#define RED_LED_PIN_N     14

#define LED_ON            1

/* Definitions for timer.c */

#define MS_TIM_N  3

#endif
