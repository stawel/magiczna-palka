/* This file is generated automatically. */

#define WEAK __attribute__ ((weak, alias("Default_Handler")))

/* Interrupt handler declarations */
WEAK void NMI_Handler(void);
WEAK void HardFault_Handler(void);
WEAK void SVC_Handler(void);
WEAK void PendSV_Handler(void);
WEAK void SysTick_Handler(void);
WEAK void WWDG_IRQHandler(void);
WEAK void PVD_IRQHandler(void);
WEAK void RTC_IRQHandler(void);
WEAK void FLASH_IRQHandler(void);
WEAK void RCC_IRQHandler(void);
WEAK void EXTI0_1_IRQHandler(void);
WEAK void EXTI2_3_IRQHandler(void);
WEAK void EXTI4_15_IRQHandler(void);
WEAK void TS_IRQHandler(void);
WEAK void DMA1_Channel1_IRQHandler(void);
WEAK void DMA1_Channel2_3_IRQHandler(void);
WEAK void DMA1_Channel4_5_IRQHandler(void);
WEAK void ADC1_COMP_IRQHandler(void);
WEAK void TIM1_BRK_UP_TRG_COM_IRQHandler(void);
WEAK void TIM1_CC_IRQHandler(void);
WEAK void TIM2_IRQHandler(void);
WEAK void TIM3_IRQHandler(void);
WEAK void TIM6_DAC_IRQHandler(void);
WEAK void TIM14_IRQHandler(void);
WEAK void TIM15_IRQHandler(void);
WEAK void TIM16_IRQHandler(void);
WEAK void TIM17_IRQHandler(void);
WEAK void I2C1_IRQHandler(void);
WEAK void I2C2_IRQHandler(void);
WEAK void SPI1_IRQHandler(void);
WEAK void SPI2_IRQHandler(void);
WEAK void USART1_IRQHandler(void);
WEAK void USART2_IRQHandler(void);
WEAK void CEC_IRQHandler(void);

/* Interrupt table */
__attribute__ ((section(".isr_vector")))
void (* const g_pfnVectors[])(void) = {
  (void*)&_estack,
  Reset_Handler,
  NMI_Handler,
  HardFault_Handler,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  SVC_Handler,
  0,
  0,
  PendSV_Handler,
  SysTick_Handler,
  WWDG_IRQHandler,
  PVD_IRQHandler,
  RTC_IRQHandler,
  FLASH_IRQHandler,
  RCC_IRQHandler,
  EXTI0_1_IRQHandler,
  EXTI2_3_IRQHandler,
  EXTI4_15_IRQHandler,
  TS_IRQHandler,
  DMA1_Channel1_IRQHandler,
  DMA1_Channel2_3_IRQHandler,
  DMA1_Channel4_5_IRQHandler,
  ADC1_COMP_IRQHandler,
  TIM1_BRK_UP_TRG_COM_IRQHandler,
  TIM1_CC_IRQHandler,
  TIM2_IRQHandler,
  TIM3_IRQHandler,
  TIM6_DAC_IRQHandler,
  0,
  TIM14_IRQHandler,
  TIM15_IRQHandler,
  TIM16_IRQHandler,
  TIM17_IRQHandler,
  I2C1_IRQHandler,
  I2C2_IRQHandler,
  SPI1_IRQHandler,
  SPI2_IRQHandler,
  USART1_IRQHandler,
  USART2_IRQHandler,
  0,
  CEC_IRQHandler
};

