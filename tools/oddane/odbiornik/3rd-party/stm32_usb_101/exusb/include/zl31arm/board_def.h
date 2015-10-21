#ifndef _BOARD_DEF_H
#define _BOARD_DEF_H 1

#include <misc.h>
#include <stm32f10x_exti.h>
#include <stm32f10x_flash.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_tim.h>
#include <xcat.h>

/** Prototype board based on ZL31ARM **/

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
  #error ZL31ARM board uses 8 MHz external quarz.
#endif

/* Definitions for boot_10x.c */

#define BOOT1_GPIO_N  A
#define BOOT1_PIN_N   1

/* Definitions for led.c
   Green LED is L2. Red LED is L1. */

#define GREEN_LED_GPIO_N  B
#define GREEN_LED_PIN_N   2

#define RED_LED_GPIO_N    B
#define RED_LED_PIN_N     1

#define LED_ON            0

/* Definitions for timer.c */

#define MS_TIM_N  2

/* Definitions for dac_pwm_10x.c */

#define PWM_TIM_N       3
#define PWM_TIM_CH      1
#define PWM_OUT_GPIO_N  A
#define PWM_OUT_PIN_N   6

/* Definitions for joystick_gpio_10x.c
   Left button is joystick fire. */

#define OYSTICK_L_GPIO_N       B
#define JOYSTICK_H_GPIO_N      C

#define PIN_JOYSTICK_BUTTON_1  0x00000020U
#define PIN_JOYSTICK_BUTTON_2  0x00000080U
#define PIN_JOYSTICK_BUTTON_3  0x00000040U
#define PIN_JOYSTICK_UP        0x00010000U
#define PIN_JOYSTICK_DOWN      0x00020000U
#define PIN_JOYSTICK_RIGHT     0x00080000U
#define PIN_JOYSTICK_LEFT      0x00040000U

/* Definitions for pwr_periph_10x.c */

#define POWER_LED_GPIO_N  A
#define POWER_LED_PIN_N   9
#define POWER_LED_ON      1

#define PUSH_BUTTON_GPIO_N  A
#define PUSH_BUTTON_PIN_N   0
#define PUSH_BUTTON_IRQ_N   EXTI0
#define PUSH_BUTTON_EDGE    EXTI_Trigger_Rising

#endif
