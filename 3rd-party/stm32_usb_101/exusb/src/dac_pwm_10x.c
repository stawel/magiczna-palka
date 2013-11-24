#include <board_def.h>
#include <dac.h>
#include <dac_buffer.h>
#include <irq.h>

#define PWM_TIM             xcat(TIM, PWM_TIM_N)
#define PWM_TIM_RCC         xcat(RCC_APB1Periph_TIM, PWM_TIM_N)
#define PWM_TIM_IRQn        xcat3(TIM, PWM_TIM_N, _IRQn)
#define PWM_TIM_IRQHandler  xcat3(TIM, PWM_TIM_N, _IRQHandler)

#define TIM_OCxInit           xcat3(TIM_OC, PWM_TIM_CH, Init)
#define TIM_OCxPreloadConfig  xcat3(TIM_OC, PWM_TIM_CH, PreloadConfig)
#define CCRx                  xcat(CCR, PWM_TIM_CH)

#define PWM_OUT_GPIO  xcat(GPIO, PWM_OUT_GPIO_N)
#define PWM_OUT_PIN   xcat(GPIO_Pin_, PWM_OUT_PIN_N)
#define PWM_OUT_RCC   xcat(RCC_APB2Periph_GPIO, PWM_OUT_GPIO_N)

static uint32_t period, compr;

int DACconfigure(unsigned samplingFrequency) {
  GPIO_InitTypeDef        GPIO_InitStruct;
  NVIC_InitTypeDef        NVIC_InitStruct;
  RCC_ClocksTypeDef       RCC_ClocksStruct;
  TIM_TimeBaseInitTypeDef TIM_BaseInitStruct;
  TIM_OCInitTypeDef       TIM_OCInitStruct;
  uint32_t                p;

  RCC_GetClocksFreq(&RCC_ClocksStruct);
  if (RCC_ClocksStruct.HCLK_Frequency ==
      RCC_ClocksStruct.PCLK1_Frequency) /* APB1 prescaler == 1 */
    period = RCC_ClocksStruct.PCLK1_Frequency / samplingFrequency;
  else
    period = 2 * RCC_ClocksStruct.PCLK1_Frequency / samplingFrequency;

  /* Compute the compression factor compr, i.e., the number of low
     significant bits that must be truncated from each sample. */
  for (p = 0x10000U, compr = 0; p > period; p >>= 1, ++compr);

  RCC_APB2PeriphClockCmd(PWM_OUT_RCC | RCC_APB2Periph_AFIO, ENABLE);

  GPIO_InitStruct.GPIO_Pin = PWM_OUT_PIN;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(PWM_OUT_GPIO, &GPIO_InitStruct);

  RCC_APB1PeriphClockCmd(PWM_TIM_RCC, ENABLE);

  NVIC_InitStruct.NVIC_IRQChannel = PWM_TIM_IRQn;
  NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = HIGH_IRQ_PRIO;
  NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStruct);

  TIM_TimeBaseStructInit(&TIM_BaseInitStruct);
  TIM_BaseInitStruct.TIM_Period = period - 1;
  TIM_TimeBaseInit(PWM_TIM, &TIM_BaseInitStruct);

  TIM_OCStructInit(&TIM_OCInitStruct);
  TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCxInit(PWM_TIM, &TIM_OCInitStruct);

  TIM_ARRPreloadConfig(PWM_TIM, ENABLE);
  TIM_OCxPreloadConfig(PWM_TIM, TIM_OCPreload_Enable);

  TIM_ClearITPendingBit(PWM_TIM, TIM_IT_Update);
  TIM_ITConfig(PWM_TIM, TIM_IT_Update, ENABLE);
  TIM_Cmd(PWM_TIM, ENABLE);

  return 0;
}

/* This interrupt is very time critical. */
void PWM_TIM_IRQHandler(void) {
  uint32_t p;

  /* Clear interrupt flag. */
  PWM_TIM->SR = 0;

  /* Align sample bits, remove usunsed bits. */
  PWM_TIM->CCRx = ((uint16_t)buffer[readPtr] ^ 0x8000U) >> compr;

  if (buffered > 0) {
    --buffered;
    if (readPtr < DAC_BUFF_SIZE - 1)
      ++readPtr;
    else
      readPtr = 0;
  }

  /* Heuristics to keep synchronization */
  p = period + MDEV - 1 - (buffered >> MPER);
  PWM_TIM->ARR = (uint16_t)p;
}
