#include <usbd_callbacks.h>
#include <usb_endianness.h>
#include <usbd_api.h>
#include <stm32.h>
#include <inttypes.h>
#include <stm32f4xx_adc.h>
#include <stm32f4xx.h>
#include <stm32f4xx_dma.h>
#include <stm32f4xx_gpio.h>
#include <stm32f4xx_rcc.h>
#include <misc.h>
#include <delay.h>

#define MAX 1024*8
#define ADC3_DR_ADDRESS     ((uint32_t)0x4001224C)

__IO uint8_t buf[MAX];

#define PIN2 GPIO_Pin_15
#define PIN1 GPIO_Pin_13
#define PIN0 GPIO_Pin_11

void PIN_Configuration(void)
{
  /* Enable ADC3, DMA2 and GPIO clocks ****************************************/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2 | RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE);

	GPIO_InitTypeDef      GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = PIN0 | PIN1 | PIN2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	GPIO_ResetBits(GPIOD, PIN0 | PIN1 | PIN2);
}


void ADC_Configuration(int pin)
{

	ADC_InitTypeDef       ADC_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	DMA_InitTypeDef       DMA_InitStructure;
	GPIO_InitTypeDef      GPIO_InitStructure;


	/* DMA2 Stream0 channel0 configuration **************************************/
	DMA_InitStructure.DMA_Channel = DMA_Channel_2;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)ADC3_DR_ADDRESS;
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&buf[0];
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	DMA_InitStructure.DMA_BufferSize = MAX;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
//	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
//	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
//	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA2_Stream0, &DMA_InitStructure);

	// Enable DMA1 Channel Transfer Complete interrupt
	DMA_ITConfig(DMA2_Stream0, DMA_IT_TC, ENABLE);

	NVIC_InitTypeDef NVIC_InitStructure;
	//Enable DMA1 channel IRQ Channel */
	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	DMA_Cmd(DMA2_Stream0, ENABLE);

	/* Configure ADC3 Channel12 pin as analog input ******************************/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* ADC Common Init **********************************************************/
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
	ADC_CommonInit(&ADC_CommonInitStructure);

	/* ADC3 Init ****************************************************************/
//	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_8b;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfConversion = 1;
	ADC_Init(ADC3, &ADC_InitStructure);

	/* ADC3 regular channel12 configuration *************************************/
	ADC_RegularChannelConfig(ADC3, ADC_Channel_12, 1, ADC_SampleTime_3Cycles);

	/* Enable DMA request after last transfer (Single-ADC mode) */
	ADC_DMARequestAfterLastTransferCmd(ADC3, ENABLE);

	/* Enable ADC3 DMA */
	ADC_DMACmd(ADC3, ENABLE);

	/* Enable ADC3 */
	ADC_Cmd(ADC3, ENABLE);


	/* Start ADC3 Software Conversion */
	if(pin &1) GPIO_SetBits(GPIOD,   PIN1);
	else	   GPIO_ResetBits(GPIOD, PIN1);
	if(pin &2) GPIO_SetBits(GPIOD,   PIN2);
	else	   GPIO_ResetBits(GPIOD, PIN2);

	__disable_irq();
	GPIO_ResetBits(GPIOD, PIN0);
	ADC_SoftwareStartConv(ADC3);
	__enable_irq();
	Delay(50);
	GPIO_SetBits(GPIOD, PIN0);
}

void DMA2_Stream0_IRQHandler(void)
{
  //Test on DMA1 Channel1 Transfer Complete interrupt
  if(DMA_GetITStatus(DMA2_Stream0, DMA_IT_TCIF0))
  {
	DMA_ClearITPendingBit(DMA2_Stream0, DMA_FLAG_TCIF0);
	ADC_DMACmd(ADC3, DISABLE);

	/* Enable ADC3 */
	ADC_Cmd(ADC3, DISABLE);
	USBDwriteEx(ENDP1, (uint8_t const *)buf, MAX);

   //Clear DMA1 Channel1 Half Transfer, Transfer Complete and Global interrupt pending bits
  }
}

void adcStart(int c)
{
	ADC_Configuration(c);
}
