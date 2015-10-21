#ifndef _BOARD_DEF_H
#define _BOARD_DEF_H 1

#include <misc.h>
#include <stm32f10x_exti.h>
#include <stm32f10x_flash.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_tim.h>
#include <xcat.h>

/** ZL30ARM board **/

/* Wszystkie stałe powinny być definiowane w plikach nagłówkowych,
   ale Standard Peripheral Library na to nie pozwala... */
#ifndef USE_STDPERIPH_DRIVER
  #error USE_STDPERIPH_DRIVER not defined
#endif
#ifndef STM32F10X_MD
  #error STM32F10X_MD not defined
#endif
#ifndef HSE_VALUE
  #error HSE_VALUE not defined
#endif

/* Zapewnij spójność stałych definiowanych w różnych miejscach. */
#if HSE_VALUE != 8000000
  #error ZL30ARM board uses 8 MHz external quarz.
#endif

/* Definitions for boot_10x.c */

#define BOOT1_GPIO_N  B
#define BOOT1_PIN_N   2

/* Definitions for lcd_hd44780_10x.c */

#define LCD_DATA_GPIO_N  B
#define LCD_D0_PIN_N     8

#define LCD_CTRL_GPIO_N  B
#define LCD_RS_PIN_N     5
#define LCD_E_PIN_N      6
#define LCD_RW_PIN_N     7

/* Definitions for led.c
   Green LED is D6. Red LED is D7. */

#define GREEN_LED_GPIO_N  B
#define GREEN_LED_PIN_N   1

#define RED_LED_GPIO_N    B
#define RED_LED_PIN_N     0

#define LED_ON            1

/* Definitions for timer.c */

#define MS_TIM_N  2

/* Definitions for dac_pwm_10x.c */

#define PWM_TIM_N       3
#define PWM_TIM_CH      1
#define PWM_OUT_GPIO_N  A
#define PWM_OUT_PIN_N   6

/* Definitions for joystick_gpio_10x.c
   Left button is joystick fire, right is SW2, middle is SW3. */

#define JOYSTICK_L_GPIO_N      A
#define JOYSTICK_H_GPIO_N      C

#define PIN_JOYSTICK_BUTTON_1  0x00000200U
#define PIN_JOYSTICK_BUTTON_2  0x20000000U
#define PIN_JOYSTICK_BUTTON_3  0x80000000U
#define PIN_JOYSTICK_UP        0x00000010U
#define PIN_JOYSTICK_DOWN      0x00000020U
#define PIN_JOYSTICK_RIGHT     0x00000080U
#define PIN_JOYSTICK_LEFT      0x00000100U

/* Definitions for pwr_periph_10x.c
   Power LED is D0. Wakeup button is SW3. */

#define POWER_LED_GPIO_N  A
#define POWER_LED_PIN_N   1
#define POWER_LED_ON      1

#define PUSH_BUTTON_GPIO_N  C
#define PUSH_BUTTON_PIN_N   15
#define PUSH_BUTTON_IRQ_N   EXTI15_10
#define PUSH_BUTTON_EDGE    EXTI_Trigger_Falling

#endif
