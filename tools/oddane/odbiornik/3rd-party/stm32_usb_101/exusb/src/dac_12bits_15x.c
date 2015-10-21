#include <dac.h>
#include <dac_buffer.h>
#include <irq.h>
#include <misc.h>

#include <stm32l1xx_dac.h>
#include <stm32l1xx_gpio.h>
#include <stm32l1xx_rcc.h>
#include <stm32l1xx_tim.h>

static uint32_t period;

int DACconfigure(unsigned samplingFrequency) {
  DAC_InitTypeDef   DAC_InitStruct;
  GPIO_InitTypeDef  GPIO_InitStruct;
  NVIC_InitTypeDef  NVIC_InitStruct;
  RCC_ClocksTypeDef RCC_ClocksStruct;

  RCC_GetClocksFreq(&RCC_ClocksStruct);
  if (RCC_ClocksStruct.HCLK_Frequency ==
      RCC_ClocksStruct.PCLK1_Frequency) /* APB1 prescaler == 1 */
    period = RCC_ClocksStruct.PCLK1_Frequency / samplingFrequency;
  else
    period = 2 * RCC_ClocksStruct.PCLK1_Frequency / samplingFrequency;

  /* Once the DAC channel is enabled, the corresponding GPIO pin is
     automatically connected to the DAC converter. In order to avoid
     parasitic consumption, the GPIO pin should be configured as
     analog. */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

  GPIO_StructInit(&GPIO_InitStruct);
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4; /* DAC OUT1 */
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN;
  GPIO_Init(GPIOA, &GPIO_InitStruct);

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6 | RCC_APB1Periph_DAC,
                         ENABLE);

  DAC_StructInit(&DAC_InitStruct);
  DAC_InitStruct.DAC_Trigger = DAC_Trigger_T6_TRGO;
  /* DAC_InitStruct.DAC_OutputBuffer = DAC_OutputBuffer_Enable; default */
  DAC_Init(DAC_Channel_1, &DAC_InitStruct);
  DAC_Cmd(DAC_Channel_1, ENABLE);

  NVIC_InitStruct.NVIC_IRQChannel = TIM6_IRQn;
  NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = HIGH_IRQ_PRIO;
  NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStruct);

  TIM_PrescalerConfig(TIM6, 0, TIM_PSCReloadMode_Immediate);
  TIM_SetAutoreload(TIM6, period - 1);
  TIM_ARRPreloadConfig(TIM6, ENABLE);
  TIM_SelectOutputTrigger(TIM6, TIM_TRGOSource_Update);
  TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
  TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);
  TIM_Cmd(TIM6, ENABLE);

  return 0;
}

/* This interrupt is very time critical. */
void TIM6_IRQHandler(void) {
  uint32_t p;

  /* Clear interrupt flag. */
  TIM6->SR = 0;

  /* 12 bits left aligned */
  *(__IO int32_t *)(DAC_BASE + 12) = (int32_t)buffer[readPtr] + 32768;

  if (buffered > 0) {
    --buffered;
    if (readPtr < DAC_BUFF_SIZE - 1)
      ++readPtr;
    else
      readPtr = 0;
  }

  /* Heuristics to keep synchronization */
  p = period + MDEV - 1 - (buffered >> MPER);
  TIM6->ARR = (uint16_t)p;
}
