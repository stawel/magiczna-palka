/* This file is generated automatically. */

#define WEAK __attribute__ ((weak, alias("Default_Handler")))

/* Interrupt handler declarations */
WEAK void NMI_Handler(void);
WEAK void HardFault_Handler(void);
WEAK void MemManage_Handler(void);
WEAK void BusFault_Handler(void);
WEAK void UsageFault_Handler(void);
WEAK void SVC_Handler(void);
WEAK void DebugMon_Handler(void);
WEAK void PendSV_Handler(void);
WEAK void SysTick_Handler(void);
WEAK void WWDG_IRQHandler(void);
WEAK void PVD_IRQHandler(void);
WEAK void TAMPER_STAMP_IRQHandler(void);
WEAK void RTC_WKUP_IRQHandler(void);
WEAK void FLASH_IRQHandler(void);
WEAK void RCC_IRQHandler(void);
WEAK void EXTI0_IRQHandler(void);
WEAK void EXTI1_IRQHandler(void);
WEAK void EXTI2_IRQHandler(void);
WEAK void EXTI3_IRQHandler(void);
WEAK void EXTI4_IRQHandler(void);
WEAK void DMA1_Channel1_IRQHandler(void);
WEAK void DMA1_Channel2_IRQHandler(void);
WEAK void DMA1_Channel3_IRQHandler(void);
WEAK void DMA1_Channel4_IRQHandler(void);
WEAK void DMA1_Channel5_IRQHandler(void);
WEAK void DMA1_Channel6_IRQHandler(void);
WEAK void DMA1_Channel7_IRQHandler(void);
WEAK void ADC1_IRQHandler(void);
WEAK void USB_HP_IRQHandler(void);
WEAK void USB_LP_IRQHandler(void);
WEAK void DAC_IRQHandler(void);
WEAK void COMP_IRQHandler(void);
WEAK void EXTI9_5_IRQHandler(void);
WEAK void LCD_IRQHandler(void);
WEAK void TIM9_IRQHandler(void);
WEAK void TIM10_IRQHandler(void);
WEAK void TIM11_IRQHandler(void);
WEAK void TIM2_IRQHandler(void);
WEAK void TIM3_IRQHandler(void);
WEAK void TIM4_IRQHandler(void);
WEAK void I2C1_EV_IRQHandler(void);
WEAK void I2C1_ER_IRQHandler(void);
WEAK void I2C2_EV_IRQHandler(void);
WEAK void I2C2_ER_IRQHandler(void);
WEAK void SPI1_IRQHandler(void);
WEAK void SPI2_IRQHandler(void);
WEAK void USART1_IRQHandler(void);
WEAK void USART2_IRQHandler(void);
WEAK void USART3_IRQHandler(void);
WEAK void EXTI15_10_IRQHandler(void);
WEAK void RTC_Alarm_IRQHandler(void);
WEAK void USB_FS_WKUP_IRQHandler(void);
WEAK void TIM6_IRQHandler(void);
WEAK void TIM7_IRQHandler(void);
WEAK void SDIO_IRQHandler(void);
WEAK void TIM5_IRQHandler(void);
WEAK void SPI3_IRQHandler(void);
WEAK void UART4_IRQHandler(void);
WEAK void UART5_IRQHandler(void);
WEAK void DMA2_Channel1_IRQHandler(void);
WEAK void DMA2_Channel2_IRQHandler(void);
WEAK void DMA2_Channel3_IRQHandler(void);
WEAK void DMA2_Channel4_IRQHandler(void);
WEAK void DMA2_Channel5_IRQHandler(void);
WEAK void AES_IRQHandler(void);
WEAK void COMP_ACQ_IRQHandler(void);

/* Interrupt table */
__attribute__ ((section(".isr_vector")))
void (* const g_pfnVectors[])(void) = {
  (void*)&_estack,
  Reset_Handler,
  NMI_Handler,
  HardFault_Handler,
  MemManage_Handler,
  BusFault_Handler,
  UsageFault_Handler,
  0,
  0,
  0,
  0,
  SVC_Handler,
  DebugMon_Handler,
  0,
  PendSV_Handler,
  SysTick_Handler,
  WWDG_IRQHandler,
  PVD_IRQHandler,
  TAMPER_STAMP_IRQHandler,
  RTC_WKUP_IRQHandler,
  FLASH_IRQHandler,
  RCC_IRQHandler,
  EXTI0_IRQHandler,
  EXTI1_IRQHandler,
  EXTI2_IRQHandler,
  EXTI3_IRQHandler,
  EXTI4_IRQHandler,
  DMA1_Channel1_IRQHandler,
  DMA1_Channel2_IRQHandler,
  DMA1_Channel3_IRQHandler,
  DMA1_Channel4_IRQHandler,
  DMA1_Channel5_IRQHandler,
  DMA1_Channel6_IRQHandler,
  DMA1_Channel7_IRQHandler,
  ADC1_IRQHandler,
  USB_HP_IRQHandler,
  USB_LP_IRQHandler,
  DAC_IRQHandler,
  COMP_IRQHandler,
  EXTI9_5_IRQHandler,
  LCD_IRQHandler,
  TIM9_IRQHandler,
  TIM10_IRQHandler,
  TIM11_IRQHandler,
  TIM2_IRQHandler,
  TIM3_IRQHandler,
  TIM4_IRQHandler,
  I2C1_EV_IRQHandler,
  I2C1_ER_IRQHandler,
  I2C2_EV_IRQHandler,
  I2C2_ER_IRQHandler,
  SPI1_IRQHandler,
  SPI2_IRQHandler,
  USART1_IRQHandler,
  USART2_IRQHandler,
  USART3_IRQHandler,
  EXTI15_10_IRQHandler,
  RTC_Alarm_IRQHandler,
  USB_FS_WKUP_IRQHandler,
  TIM6_IRQHandler,
  TIM7_IRQHandler,
  SDIO_IRQHandler,
  TIM5_IRQHandler,
  SPI3_IRQHandler,
  UART4_IRQHandler,
  UART5_IRQHandler,
  DMA2_Channel1_IRQHandler,
  DMA2_Channel2_IRQHandler,
  DMA2_Channel3_IRQHandler,
  DMA2_Channel4_IRQHandler,
  DMA2_Channel5_IRQHandler,
  AES_IRQHandler,
  COMP_ACQ_IRQHandler
};

