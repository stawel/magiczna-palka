#include <active_check.h>
#include <board_def.h>
#include <board_init.h>

/** STM32F205, STM32F207, STM32F215 and STM32F217 initialization, also
    applicable for STM32F405, STM32F407, STM32F415 and STM32F417. **/

static GPIO_TypeDef * const gpio[] = {
  GPIOA, GPIOB, GPIOC, GPIOD, GPIOE, GPIOF, GPIOG, GPIOH, GPIOI
};
static const int gpioCount = sizeof(gpio)/sizeof(gpio[0]);
static const uint32_t allRccGpio = RCC_AHB1Periph_GPIOA |
                                   RCC_AHB1Periph_GPIOB |
                                   RCC_AHB1Periph_GPIOC |
                                   RCC_AHB1Periph_GPIOD |
                                   RCC_AHB1Periph_GPIOE |
                                   RCC_AHB1Periph_GPIOF |
                                   RCC_AHB1Periph_GPIOG |
                                   RCC_AHB1Periph_GPIOH |
                                   RCC_AHB1Periph_GPIOI;

/* Configure all leads as analog inputs, which reduces power
   consumption and electromagnetic interferences. */
void AllPinsDisable() {
  GPIO_InitTypeDef GPIO_InitStruct;
  int i;

  RCC_AHB1PeriphClockCmd(allRccGpio, ENABLE);

  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;

  /* JTAG is connected to PA13, PA14 and PA15. */
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0  | GPIO_Pin_1  | GPIO_Pin_2  |
                             GPIO_Pin_3  | GPIO_Pin_4  | GPIO_Pin_5  |
                             GPIO_Pin_6  | GPIO_Pin_7  | GPIO_Pin_8  |
                             GPIO_Pin_9  | GPIO_Pin_10 | GPIO_Pin_11 |
                             GPIO_Pin_12;
  GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* JTAG is connected to PB3 and PB4. */
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0  | GPIO_Pin_1  | GPIO_Pin_2  |
                             GPIO_Pin_5  | GPIO_Pin_6  | GPIO_Pin_7  |
                             GPIO_Pin_8  | GPIO_Pin_9  | GPIO_Pin_10 |
                             GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 |
                             GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_Init(GPIOB, &GPIO_InitStruct);

  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_All;
  for (i = 2; i < gpioCount; ++i)
    GPIO_Init(gpio[i], &GPIO_InitStruct);

  RCC_AHB1PeriphClockCmd(allRccGpio, DISABLE);
}

#ifndef HSE_BYPASS
  #define HSE_BYPASS  0
#endif

int ClockConfigure(int sysclk_MHz) {
  static const int maxAttempts = 1000000;

  RCC_DeInit();
  if (HSE_BYPASS)
    RCC_HSEConfig(RCC_HSE_Bypass);
  else
    RCC_HSEConfig(RCC_HSE_ON);
  /* Try maximum HSEStartUp_TimeOut == 0x0500 times. */
  if (RCC_WaitForHSEStartUp() == ERROR)
    return -1;

  if (HSE_VALUE >= 4000000 && HSE_VALUE <= 26000000 &&
      HSE_VALUE % 1000000 == 0) {
    uint32_t M = HSE_VALUE / 1000000; /* PLLs input clock is 1 MHz. */
    if (sysclk_MHz == 48)
      RCC_PLLConfig(RCC_PLLSource_HSE, M, 192, 4, 4);
    else if (sysclk_MHz == 72)
      RCC_PLLConfig(RCC_PLLSource_HSE, M, 288, 4, 6);
    else if (sysclk_MHz == 120)
      RCC_PLLConfig(RCC_PLLSource_HSE, M, 240, 2, 5);
    else if (sysclk_MHz == 168)
      RCC_PLLConfig(RCC_PLLSource_HSE, M, 336, 2, 7);
    else
      return -1;
  }
  else
    return -1;

  RCC_PLLCmd(ENABLE);
  active_check(RCC_GetFlagStatus(RCC_FLAG_PLLRDY), maxAttempts);

  FLASH_PrefetchBufferCmd(ENABLE);
  FLASH_InstructionCacheCmd(ENABLE);
  FLASH_DataCacheCmd(ENABLE);
  /* We assume supply voltage > 2.7 V. */
  if (sysclk_MHz > 150)
    FLASH_SetLatency(FLASH_Latency_5);
  else if (sysclk_MHz > 120)
    FLASH_SetLatency(FLASH_Latency_4);
  else if (sysclk_MHz > 90)
    FLASH_SetLatency(FLASH_Latency_3);
  else if (sysclk_MHz > 60)
    FLASH_SetLatency(FLASH_Latency_2);
  else if (sysclk_MHz > 30)
    FLASH_SetLatency(FLASH_Latency_1);
  else
    FLASH_SetLatency(FLASH_Latency_0);

  /* Set AHB prescaler, HCLK = SYSCLK. */
  RCC_HCLKConfig(RCC_SYSCLK_Div1);
  /* Set APB1 prescaler, PCLK1 = HCLK / 4. */
  RCC_PCLK1Config(RCC_HCLK_Div4);
  /* Set APB2 prescaler, PCLK2 = HCLK / 2. */
  RCC_PCLK2Config(RCC_HCLK_Div2);

  /* Set SYSCLK = PLLCLK. */
  RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
  active_check(RCC_GetSYSCLKSource() == 0x08, maxAttempts);

  /* I2S clock configuration (for devices Rev B and Y)
     I2SCLK = ((HSE_VALUE / PLL_M) * PLLI2S_N) / PLLI2S_R
     HSE_VALUE / PLL_M = 1 MHz, I2SCLK = 172 MHz */
  RCC_I2SCLKConfig(RCC_I2S2CLKSource_PLLI2S);
  RCC_PLLI2SConfig(344, 2);
  RCC_PLLI2SCmd(ENABLE);
  active_check(RCC_GetFlagStatus(RCC_FLAG_PLLI2SRDY), maxAttempts);

  return 0;
}
