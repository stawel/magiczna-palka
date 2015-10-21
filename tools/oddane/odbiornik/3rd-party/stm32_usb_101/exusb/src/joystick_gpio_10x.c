#include <board_def.h>
#include <joystick.h>

#define GPIO_JOYSTICK_L  xcat(GPIO, JOYSTICK_L_GPIO_N)
#define RCC_JOYSTICK_L   xcat(RCC_APB2Periph_GPIO, JOYSTICK_L_GPIO_N)

#ifdef JOYSTICK_H_GPIO_N
  #define GPIO_JOYSTICK_H  xcat(GPIO, JOYSTICK_H_GPIO_N)
  #define RCC_JOYSTICK_H   xcat(RCC_APB2Periph_GPIO, JOYSTICK_H_GPIO_N)
#else
  #define GPIO_JOYSTICK_H  0
  #define RCC_JOYSTICK_H   0
#endif

#define PIN_JOYSTICK_ALL                                          \
  (PIN_JOYSTICK_BUTTON_1 | PIN_JOYSTICK_BUTTON_2 |                \
   PIN_JOYSTICK_BUTTON_3 | PIN_JOYSTICK_UP | PIN_JOYSTICK_DOWN |  \
   PIN_JOYSTICK_RIGHT | PIN_JOYSTICK_LEFT)

int JoystickConfigure() {
  GPIO_InitTypeDef GPIO_InitStruct;

  RCC_APB2PeriphClockCmd(RCC_JOYSTICK_L | RCC_JOYSTICK_H, ENABLE);

  GPIO_InitStruct.GPIO_Pin = PIN_JOYSTICK_ALL & 0xffff;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_InitStruct.GPIO_Speed = 0;
  GPIO_Init(GPIO_JOYSTICK_L, &GPIO_InitStruct);
  if (GPIO_JOYSTICK_H) {
    GPIO_InitStruct.GPIO_Pin = PIN_JOYSTICK_ALL >> 16;
    GPIO_Init(GPIO_JOYSTICK_H, &GPIO_InitStruct);
  }

  return 0;
}

int JoystickReset() {
  return 0;
}

unsigned JoystickGetState() {
  uint32_t pin_state;
  unsigned joy_state;

  pin_state = ~GPIO_ReadInputData(GPIO_JOYSTICK_L) & 0xffff;
  if (GPIO_JOYSTICK_H)
    pin_state |= ~GPIO_ReadInputData(GPIO_JOYSTICK_H) << 16;

  joy_state = 0;
  if (pin_state & PIN_JOYSTICK_BUTTON_1)
    joy_state |= JOYSTICK_BUTTON_1;
  if (pin_state & PIN_JOYSTICK_BUTTON_2)
    joy_state |= JOYSTICK_BUTTON_2;
  if (pin_state & PIN_JOYSTICK_BUTTON_3)
    joy_state |= JOYSTICK_BUTTON_3;
  if (pin_state & PIN_JOYSTICK_UP)
    joy_state |= JOYSTICK_UP;
  if (pin_state & PIN_JOYSTICK_DOWN)
    joy_state |= JOYSTICK_DOWN;
  if (pin_state & PIN_JOYSTICK_RIGHT)
    joy_state |= JOYSTICK_RIGHT;
  if (pin_state & PIN_JOYSTICK_LEFT)
    joy_state |= JOYSTICK_LEFT;

  return joy_state;
}
