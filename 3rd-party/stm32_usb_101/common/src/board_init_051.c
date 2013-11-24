#include <active_check.h>
#include <board_def.h>
#include <board_init.h>

/** STM32F051 initialization **/

static GPIO_TypeDef * const gpio[] = {
  GPIOA, GPIOB, GPIOC, GPIOD
};
static const int gpioCount = sizeof(gpio)/sizeof(gpio[0]);
static const uint32_t allRccGpio = RCC_AHBPeriph_GPIOA |
                                   RCC_AHBPeriph_GPIOB |
                                   RCC_AHBPeriph_GPIOC |
                                   RCC_AHBPeriph_GPIOD;

/* Configure all leads as analog inputs, which reduces power
   consumption and electromagnetic interferences. */
void AllPinsDisable() {
  GPIO_InitTypeDef GPIO_InitStruct;
  int i;

  RCC_AHBPeriphClockCmd(allRccGpio, ENABLE);

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

  RCC_AHBPeriphClockCmd(allRccGpio, DISABLE);
}

#ifndef HSE_BYPASS
  #define HSE_BYPASS  0
#endif

static void PLLconfig(uint32_t source, uint32_t div, uint32_t mul) {
  RCC_PLLConfig(source, mul);
  if (source == RCC_PLLSource_PREDIV1)
    RCC_PREDIV1Config(div);
}

int ClockConfigure(int sysclk_MHz) {
  static const int maxAttempts = 1000000;

  RCC_DeInit();
  if (HSE_VALUE > 0) {
    /* For STM32F0-Discovery board, if uC is clocked from ST-LINK/V2
       (SB18 closed), use RCC_HSE_Bypass instead of RCC_HSE_ON.
       In this case define HSE_BYPASS equal to 1 in the appriopriate
       board_def.h file. */
    if (HSE_BYPASS)
      RCC_HSEConfig(RCC_HSE_Bypass);
    else
      RCC_HSEConfig(RCC_HSE_ON);
    /* Try maximum HSEStartUp_TimeOut == 0x0500 times. */
    if (RCC_WaitForHSEStartUp() == ERROR)
      return -1;
  }

  if (HSE_VALUE == 0 && HSI_VALUE == 8000000 && sysclk_MHz == 24)
    PLLconfig(RCC_PLLSource_HSI_Div2, 0, RCC_PLLMul_6);
  else if (HSE_VALUE == 0 && HSI_VALUE == 8000000 && sysclk_MHz == 32)
    PLLconfig(RCC_PLLSource_HSI_Div2, 0, RCC_PLLMul_8);
  else if (HSE_VALUE == 0 && HSI_VALUE == 8000000 && sysclk_MHz == 48)
    PLLconfig(RCC_PLLSource_HSI_Div2, 0, RCC_PLLMul_12);

  else if (HSE_VALUE == 8000000 && sysclk_MHz == 24)
    PLLconfig(RCC_PLLSource_PREDIV1, RCC_PREDIV1_Div1, RCC_PLLMul_3);
  else if (HSE_VALUE == 8000000 && sysclk_MHz == 32)
    PLLconfig(RCC_PLLSource_PREDIV1, RCC_PREDIV1_Div1, RCC_PLLMul_4);
  else if (HSE_VALUE == 8000000 && sysclk_MHz == 48)
    PLLconfig(RCC_PLLSource_PREDIV1, RCC_PREDIV1_Div1, RCC_PLLMul_6);

  else if (HSE_VALUE == 12000000 && sysclk_MHz == 24)
    PLLconfig(RCC_PLLSource_PREDIV1, RCC_PREDIV1_Div2, RCC_PLLMul_4);
  else if (HSE_VALUE == 12000000 && sysclk_MHz == 32)
    PLLconfig(RCC_PLLSource_PREDIV1, RCC_PREDIV1_Div3, RCC_PLLMul_8);
  else if (HSE_VALUE == 12000000 && sysclk_MHz == 48)
    PLLconfig(RCC_PLLSource_PREDIV1, RCC_PREDIV1_Div2, RCC_PLLMul_8);

  else if (HSE_VALUE == 16000000 && sysclk_MHz == 24)
    PLLconfig(RCC_PLLSource_PREDIV1, RCC_PREDIV1_Div2, RCC_PLLMul_3);
  else if (HSE_VALUE == 16000000 && sysclk_MHz == 32)
    PLLconfig(RCC_PLLSource_PREDIV1, RCC_PREDIV1_Div2, RCC_PLLMul_4);
  else if (HSE_VALUE == 16000000 && sysclk_MHz == 48)
    PLLconfig(RCC_PLLSource_PREDIV1, RCC_PREDIV1_Div2, RCC_PLLMul_6);

  else
    return -1;

  RCC_PLLCmd(ENABLE);
  active_check(RCC_GetFlagStatus(RCC_FLAG_PLLRDY), maxAttempts);

  FLASH_PrefetchBufferCmd(ENABLE);
  if (sysclk_MHz > 24)
    FLASH_SetLatency(FLASH_Latency_1);
  else
    FLASH_SetLatency(FLASH_Latency_0);

  /* Set AHB prescaler, HCLK = SYSCLK. */
  RCC_HCLKConfig(RCC_SYSCLK_Div1);
  /* Set APB prescaler, PCLK = HCLK. */
  RCC_PCLKConfig(RCC_HCLK_Div1);
  /* Set SYSCLK = PLLCLK. */
  RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
  active_check(RCC_GetSYSCLKSource() == 0x08, maxAttempts);

  return 0;
}
