#ifndef _BOARD_DEF_H
#define _BOARD_DEF_H 1

#include <misc.h>
#include <stm32f10x_exti.h>
#include <stm32f10x_flash.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_tim.h>
#include <xcat.h>

/** ZL29ARM board **/

/* Wszystkie stałe powinny być definiowane w plikach nagłówkowych,
   ale Standard Peripheral Library na to nie pozwala... */
#ifndef USE_STDPERIPH_DRIVER
  #error USE_STDPERIPH_DRIVER not defined
#endif
#ifndef STM32F10X_CL
  #error STM32F10X_CL not defined
#endif
#ifndef HSE_VALUE
  #error HSE_VALUE not defined
#endif

/* Zapewnij spójność stałych definiowanych w różnych miejscach. */
#if HSE_VALUE != 10000000
  #error ZL29ARM board uses 10 MHz external quarz.
#endif

/* Definitions for boot_10x.c */

#define BOOT1_GPIO_N  B
#define BOOT1_PIN_N   2

/* Definitions for lcd_ks0108_10x.c */

#define LCD_DATA_GPIO_N  E
#define LCD_D0_PIN_N     0

#define LCD_CTRL_GPIO_N  E
#define LCD_RS_PIN_N     8
#define LCD_E_PIN_N      9
#define LCD_RW_PIN_N     10
#define LCD_CS1_PIN_N    11
#define LCD_CS2_PIN_N    12

/* Definitions for led.c
   Green LED is D2. Red LED is D3. */

#define GREEN_LED_GPIO_N  E
#define GREEN_LED_PIN_N   14

#define RED_LED_GPIO_N    E
#define RED_LED_PIN_N     15

#define LED_ON            0

/* Definitions for timer.c */

#define MS_TIM_N  2
#define US_TIM_N  3

/* Definitions for joystick_gpio_10x.c
   Left button is joystick fire, right is SW3, middle is SW4. */

#define JOYSTICK_L_GPIO_N      C

#define PIN_JOYSTICK_BUTTON_1  0x00000200U
#define PIN_JOYSTICK_BUTTON_2  0x00001000U
#define PIN_JOYSTICK_BUTTON_3  0x00002000U
#define PIN_JOYSTICK_UP        0x00000020U
#define PIN_JOYSTICK_DOWN      0x00000040U
#define PIN_JOYSTICK_RIGHT     0x00000080U
#define PIN_JOYSTICK_LEFT      0x00000100U

/* Definitions for pwr_periph_10x.c
   Power LED is the same as green LED. Wakeup button is SW4. */

#define POWER_LED_GPIO_N  GREEN_LED_GPIO_N
#define POWER_LED_PIN_N   GREEN_LED_PIN_N
#define POWER_LED_ON      LED_ON

#define PUSH_BUTTON_GPIO_N  C
#define PUSH_BUTTON_PIN_N   13
#define PUSH_BUTTON_IRQ_N   EXTI15_10
#define PUSH_BUTTON_EDGE    EXTI_Trigger_Falling

#endif
