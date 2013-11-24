#include <joystick.h>
#include <lis302dl.h>

#include <stm32f4xx_gpio.h>
#include <stm32f4xx_rcc.h>

/* STM32F4-DISCOVERY hardware definitions
   User key is used as the left mouse button. */
#define RCC_BUTTON1   RCC_AHB1Periph_GPIOA
#define GPIO_BUTTON1  GPIOA
#define PIN_BUTTON1   GPIO_Pin_0
#define BTN_PRESSED   1

static void UserButtonConfigure(void) {
  GPIO_InitTypeDef GPIO_InitStruct;

  RCC_AHB1PeriphClockCmd(RCC_BUTTON1, ENABLE);

  GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_IN;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;

  GPIO_InitStruct.GPIO_Pin = PIN_BUTTON1;
  GPIO_Init(GPIO_BUTTON1, &GPIO_InitStruct);
}

static int8_t xOffset, yOffset;

static void MemsConfigure1(void) {
  LIS302DL_InitTypeDef LIS302DL_InitStruct;

  LIS302DL_InitStruct.Output_DataRate = LIS302DL_DATARATE_100;
  LIS302DL_InitStruct.Power_Mode      = LIS302DL_LOWPOWERMODE_ACTIVE;
  LIS302DL_InitStruct.Full_Scale      = LIS302DL_FULLSCALE_2_3;
  LIS302DL_InitStruct.Self_Test       = LIS302DL_SELFTEST_NORMAL;
  LIS302DL_InitStruct.Axes_Enable     = LIS302DL_XY_ENABLE;
  LIS302DL_Init(&LIS302DL_InitStruct);
}

static void MemsConfigure2(void) {
  LIS302DL_FilterConfigTypeDef LIS302DL_FilterStruct;
  int8_t                       buffer[2];

  LIS302DL_FilterStruct.HighPassFilter_Data_Selection   = LIS302DL_FILTEREDDATASELECTION_OUTPUTREGISTER;
  LIS302DL_FilterStruct.HighPassFilter_Interrupt        = LIS302DL_HIGHPASSFILTERINTERRUPT_1_2;
  LIS302DL_FilterStruct.HighPassFilter_CutOff_Frequency = LIS302DL_HIGHPASSFILTER_LEVEL_1;
  LIS302DL_FilterConfig(&LIS302DL_FilterStruct);

  LIS302DL_ReadXYZ(buffer, sizeof buffer);
  xOffset = buffer[0];
  yOffset = buffer[1];
}

/* Required delay between calling to MemsConfigure1 and MemsConfigure2
   is MEMS turn-on time = 3 / output data rate = 3 / 100 = 30 ms. */

int JoystickConfigure() {
  MemsConfigure1();
  UserButtonConfigure();
  return 0;
}

int JoystickReset() {
  MemsConfigure2();
  return 0;
}

unsigned JoystickGetState() {
  static const int8_t treshold = 6;
  unsigned joy_state;
  int8_t   buffer[2];

  joy_state = 0;
  if (GPIO_ReadInputDataBit(GPIO_BUTTON1, PIN_BUTTON1) == BTN_PRESSED)
    joy_state |= JOYSTICK_BUTTON_1;

  LIS302DL_ReadXYZ(buffer, sizeof buffer);
  buffer[0] -= xOffset;
  buffer[1] -= yOffset;
  if (buffer[0] < -treshold)
    joy_state |= JOYSTICK_UP;
  else if (buffer[0] > treshold)
    joy_state |= JOYSTICK_DOWN;
  if (buffer[1] < -treshold)
    joy_state |= JOYSTICK_LEFT;
  else if(buffer[1] > treshold)
    joy_state |= JOYSTICK_RIGHT;

  return joy_state;
}
