#include <active_check.h>
#include <board_def.h>
#include <board_init.h>

/** STM32F105 and STM32F107 initialization **/

static GPIO_TypeDef * const gpio[] = {
  GPIOA, GPIOB, GPIOC, GPIOD, GPIOE, GPIOF, GPIOG
};
static const int gpioCount = sizeof(gpio)/sizeof(gpio[0]);
static const uint32_t allRccGpio = RCC_APB2Periph_GPIOA |
                                   RCC_APB2Periph_GPIOB |
                                   RCC_APB2Periph_GPIOC |
                                   RCC_APB2Periph_GPIOD |
                                   RCC_APB2Periph_GPIOE |
                                   RCC_APB2Periph_GPIOF |
                                   RCC_APB2Periph_GPIOG;

/* Configure all leads as analog inputs with trigger off, which
   reduces power consumption and electromagnetic interferences. */
void AllPinsDisable() {
  GPIO_InitTypeDef GPIO_InitStruct;
  int i;

  RCC_APB2PeriphClockCmd(allRccGpio, ENABLE);

  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_All;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_InitStruct.GPIO_Speed = 0;
  for (i = 0; i < gpioCount; ++i)
    GPIO_Init(gpio[i], &GPIO_InitStruct);

  RCC_APB2PeriphClockCmd(allRccGpio, DISABLE);
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

  if (HSE_VALUE % 5000000 == 0) {
    /* PLL2, PLL2CLK = HSE / RCC_PREDIV2_DivX * 8 = 40 MHz */
    if (HSE_VALUE == 5000000)
      RCC_PREDIV2Config(RCC_PREDIV2_Div1);
    else if (HSE_VALUE == 10000000)
      RCC_PREDIV2Config(RCC_PREDIV2_Div2);
    else if (HSE_VALUE == 15000000)
      RCC_PREDIV2Config(RCC_PREDIV2_Div3);
    else if (HSE_VALUE == 20000000)
      RCC_PREDIV2Config(RCC_PREDIV2_Div4);
    else if (HSE_VALUE == 25000000)
      RCC_PREDIV2Config(RCC_PREDIV2_Div5);
    else
      return -1;

    RCC_PLL2Config(RCC_PLL2Mul_8);
    RCC_PLL2Cmd(ENABLE);
    active_check(RCC_GetFlagStatus(RCC_FLAG_PLL2RDY), maxAttempts);

    /* PLL1 */
    if (sysclk_MHz == 48)      /* PLLCLK = (PLL2CLK / 5) * 6 = 48 MHz */
      RCC_PLLConfig(RCC_PLLSource_PREDIV1, RCC_PLLMul_6);
    else if (sysclk_MHz == 72) /* PLLCLK = (PLL2CLK / 5) * 9 = 72 MHz */
      RCC_PLLConfig(RCC_PLLSource_PREDIV1, RCC_PLLMul_9);
    else
      return -1;
    /* RCC_PREDIV1Config must be configured after RCC_PLLConfig.
       Bit 0 in CFGR2 register overwrites bit 17 in CFGR register. */
    RCC_PREDIV1Config(RCC_PREDIV1_Source_PLL2, RCC_PREDIV1_Div5);
  }
  else if (HSE_VALUE == 8000000 && sysclk_MHz == 48) {
    RCC_PLLConfig(RCC_PLLSource_PREDIV1, RCC_PLLMul_6);
    RCC_PREDIV1Config(RCC_PREDIV1_Source_HSE, RCC_PREDIV1_Div1);
  }
  else if (HSE_VALUE == 8000000 && sysclk_MHz == 72) {
    RCC_PLLConfig(RCC_PLLSource_PREDIV1, RCC_PLLMul_9);
    RCC_PREDIV1Config(RCC_PREDIV1_Source_HSE, RCC_PREDIV1_Div1);
  }
  else if (HSE_VALUE == 12000000 && sysclk_MHz == 48) {
    RCC_PLLConfig(RCC_PLLSource_PREDIV1, RCC_PLLMul_4);
    RCC_PREDIV1Config(RCC_PREDIV1_Source_HSE, RCC_PREDIV1_Div1);
  }
  else if (HSE_VALUE == 12000000 && sysclk_MHz == 72) {
    RCC_PLLConfig(RCC_PLLSource_PREDIV1, RCC_PLLMul_6);
    RCC_PREDIV1Config(RCC_PREDIV1_Source_HSE, RCC_PREDIV1_Div1);
  }
  else if (HSE_VALUE == 16000000 && sysclk_MHz == 48) {
    RCC_PLLConfig(RCC_PLLSource_PREDIV1, RCC_PLLMul_6);
    RCC_PREDIV1Config(RCC_PREDIV1_Source_HSE, RCC_PREDIV1_Div2);
  }
  else if (HSE_VALUE == 16000000 && sysclk_MHz == 72) {
    RCC_PLLConfig(RCC_PLLSource_PREDIV1, RCC_PLLMul_9);
    RCC_PREDIV1Config(RCC_PREDIV1_Source_HSE, RCC_PREDIV1_Div2);
  }
  else {
    return -1;
  }

  RCC_PLLCmd(ENABLE);
  active_check(RCC_GetFlagStatus(RCC_FLAG_PLLRDY), maxAttempts);

  FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
  if (sysclk_MHz > 48)
    FLASH_SetLatency(FLASH_Latency_2);
  else if (sysclk_MHz > 24)
    FLASH_SetLatency(FLASH_Latency_1);
  else
    FLASH_SetLatency(FLASH_Latency_0);

  /* Set AHB prescaler, HCLK = SYSCLK. */
  RCC_HCLKConfig(RCC_SYSCLK_Div1);
  /* Set APB1 prescaler, PCLK1 = HCLK / 2. */
  RCC_PCLK1Config(RCC_HCLK_Div2);
  /* Set APB2 prescaler, PCLK2 = HCLK. */
  RCC_PCLK2Config(RCC_HCLK_Div1);

  /* Set SYSCLK = PLLCLK. */
  RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
  active_check(RCC_GetSYSCLKSource() == 0x08, maxAttempts);

  return 0;
}
