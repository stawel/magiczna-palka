#ifndef _BOARD_DEF_H
#define _BOARD_DEF_H 1

#include <misc.h>
#include <stm32f2xx_dma.h>
#include <stm32f2xx_exti.h>
#include <stm32f2xx_flash.h>
#include <stm32f2xx_gpio.h>
#include <stm32f2xx_i2c.h>
#include <stm32f2xx_rcc.h>
#include <stm32f2xx_spi.h>
#include <stm32f2xx_syscfg.h>
#include <stm32f2xx_tim.h>
#include <xcat.h>

/** STM3220G-EVAL board **/

/* Wszystkie stałe powinny być definiowane w plikach nagłówkowych,
   ale Standard Peripheral Library na to nie pozwala... */
#ifndef USE_STDPERIPH_DRIVER
  #error USE_STDPERIPH_DRIVER not defined
#endif
#ifndef STM32F2XX
  #error STM32F2XX not defined
#endif
#ifndef HSE_VALUE
  #error HSE_VALUE not defined
#endif

/* Zapewnij spójność stałych definiowanych w różnych miejscach. */
#if HSE_VALUE != 25000000
  #error STM3220G-EVAL board uses 25 MHz external quarz.
#endif

/* Definitions for boot_2xx.c */

#define BOOT1_GPIO_N  B
#define BOOT1_PIN_N   2
#define BOOT2_GPIO_N  A
#define BOOT2_PIN_N   0

/* Definitions for i2c.c */

#define I2C_N          1
#define I2C_GPIO_N     B
#define I2C_SCL_PIN_N  6
#define I2C_SDA_PIN_N  9

/* Definitions for i2s.c */

#define I2S_N           2
#define I2S_MCK_GPIO_N  C
#define I2S_MCK_PIN_N   6
#define I2S_SCK_GPIO_N  I
#define I2S_SCK_PIN_N   1
#define I2S_SD_GPIO_N   I
#define I2S_SD_PIN_N    3
#define I2S_WS_GPIO_N   I
#define I2S_WS_PIN_N    0

#define I2S_DMA_N          1
#define I2S_DMA_CHANNEL_N  0
#define I2S_DMA_STREAM_N   4

/* Definitions for led.c */

#define GREEN_LED_GPIO_N  G
#define GREEN_LED_PIN_N   6

#define RED_LED_GPIO_N    I
#define RED_LED_PIN_N     9

#define LED_ON            1

/* Definitions for timer.c */

#define MS_TIM_N  2
#define US_TIM_N  3

/* Definitions for pwr_periph_2xx.c
   Power LED is blue LED. Wakeup button is Wakeup. */

#define POWER_LED_GPIO_N  C
#define POWER_LED_PIN_N   7
#define POWER_LED_ON      1

#define PUSH_BUTTON_GPIO_N  A
#define PUSH_BUTTON_PIN_N   0
#define PUSH_BUTTON_IRQ_N   EXTI0
#define PUSH_BUTTON_EDGE    EXTI_Trigger_Rising

#endif
