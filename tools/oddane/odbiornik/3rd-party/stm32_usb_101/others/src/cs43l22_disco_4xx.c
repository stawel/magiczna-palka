#include <cs43l22.h>

#include <stm32f4xx_gpio.h>
#include <stm32f4xx_rcc.h>

#define CS43L22_RESET_CLOCK  RCC_AHB1Periph_GPIOD
#define CS43L22_RESET_PORT   GPIOD
#define CS43L22_RESET_PIN    GPIO_Pin_4

/* Initialize the GPIO which the audio chip reset is connected to. */
int CS43L22_ResetConfigure() {
  GPIO_InitTypeDef GPIO_InitStruct;

  RCC_AHB1PeriphClockCmd(CS43L22_RESET_CLOCK, ENABLE);

  GPIO_WriteBit(CS43L22_RESET_PORT, CS43L22_RESET_PIN, Bit_RESET);

  GPIO_InitStruct.GPIO_Pin = CS43L22_RESET_PIN;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(CS43L22_RESET_PORT, &GPIO_InitStruct);

  return 0;
}

/* Control the state of the audio chip reset. */
int CS43L22_Reset(int state) {
  if (state)
    GPIO_WriteBit(CS43L22_RESET_PORT, CS43L22_RESET_PIN, Bit_SET);
  else
    GPIO_WriteBit(CS43L22_RESET_PORT, CS43L22_RESET_PIN, Bit_RESET);

  return 0;
}
