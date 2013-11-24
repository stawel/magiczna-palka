#include <i2c.h>
#include <ioe_stm322xg.h>
#include <joystick.h>
#include <lcd_util.h>

#include <stm32f2xx_gpio.h>
#include <stm32f2xx_rcc.h>

/* STM3220G-EVAL hardware definitions */
#define RCC_BUTTON2   RCC_AHB1Periph_GPIOG
#define RCC_BUTTON3   RCC_AHB1Periph_GPIOC
#define GPIO_BUTTON2  GPIOG
#define GPIO_BUTTON3  GPIOC
#define PIN_BUTTON2   GPIO_Pin_15
#define PIN_BUTTON3   GPIO_Pin_13
#define BTN_PRESSED   0

static volatile unsigned joy_state;

/* Joystick state is read in the LCDrefresh function, because calling
   the IOEjoystickGetState function takes too long to be performed
   in the JoystickGetState function which is executed in the USB
   interrupt routine. LCDrefresh is executed outside any interrupt.
   Writting on LCD gives desired delay between consecutive joystick
   state reads. TODO: Instead of active read, an interrupt for
   detecting of the joystick state change should be implemented.*/
static void LCDrefresh(void) {
  unsigned new_joy_state, pin_state;

  new_joy_state = 0;
  pin_state = IOE2joystickGetState();
  if (pin_state & JOY_IO_SEL)
    new_joy_state |= JOYSTICK_BUTTON_1;
  if (pin_state & JOY_IO_UP)
    new_joy_state |= JOYSTICK_UP;
  if (pin_state & JOY_IO_DOWN)
    new_joy_state |= JOYSTICK_DOWN;
  if (pin_state & JOY_IO_RIGHT)
    new_joy_state |= JOYSTICK_RIGHT;
  if (pin_state & JOY_IO_LEFT)
    new_joy_state |= JOYSTICK_LEFT;
  if (GPIO_ReadInputDataBit(GPIO_BUTTON2, PIN_BUTTON2) == BTN_PRESSED)
    new_joy_state |= JOYSTICK_BUTTON_2;
  if (GPIO_ReadInputDataBit(GPIO_BUTTON3, PIN_BUTTON3) == BTN_PRESSED)
    new_joy_state |= JOYSTICK_BUTTON_3;

  LCDgoto(0, 0);
  LCDwrite("USB HID example\nJoystick pins ");
  LCDwriteHex(2, new_joy_state);

  joy_state = new_joy_state;
}

int JoystickConfigure() {
  GPIO_InitTypeDef GPIO_InitStruct;

  RCC_AHB1PeriphClockCmd(RCC_BUTTON2 | RCC_BUTTON3, ENABLE);

  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;

  /* Key is used as right button. */
  GPIO_InitStruct.GPIO_Pin = PIN_BUTTON2;
  GPIO_Init(GPIO_BUTTON2, &GPIO_InitStruct);

  /* Tamper is used as middle button. */
  GPIO_InitStruct.GPIO_Pin = PIN_BUTTON3;
  GPIO_Init(GPIO_BUTTON3, &GPIO_InitStruct);

  /* Joystick is used as moving device and left button. Jotstick is
     connected to the second IO expander via I2C. */
  I2Cconfigure();
  if (IOE2configure() < 0)
    return -1;

  LCDsetRefresh(LCDrefresh);

  return 0;
}

int JoystickReset() {
  return 0;
}

unsigned JoystickGetState() {
  return joy_state;
}
