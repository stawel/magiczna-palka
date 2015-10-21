#include <board_def.h>
#include <i2s.h>

#define I2S             xcat(SPI, I2S_N)
#define I2S_CLOCK       xcat(RCC_APB1Periph_SPI, I2S_N)
#define I2S_WS_PORT     xcat(GPIO, I2S_WS_GPIO_N)
#define I2S_MCK_PORT    xcat(GPIO, I2S_MCK_GPIO_N)
#define I2S_SCK_PORT    xcat(GPIO, I2S_SCK_GPIO_N)
#define I2S_SD_PORT     xcat(GPIO, I2S_SD_GPIO_N)
#define I2S_WS_PIN      xcat(GPIO_Pin_, I2S_WS_PIN_N)
#define I2S_MCK_PIN     xcat(GPIO_Pin_, I2S_MCK_PIN_N)
#define I2S_SCK_PIN     xcat(GPIO_Pin_, I2S_SCK_PIN_N)
#define I2S_SD_PIN      xcat(GPIO_Pin_, I2S_SD_PIN_N)
#define I2S_WS_SOURCE   xcat(GPIO_PinSource, I2S_WS_PIN_N)
#define I2S_MCK_SOURCE  xcat(GPIO_PinSource, I2S_MCK_PIN_N)
#define I2S_SCK_SOURCE  xcat(GPIO_PinSource, I2S_SCK_PIN_N)
#define I2S_SD_SOURCE   xcat(GPIO_PinSource, I2S_SD_PIN_N)
#define I2S_AF          xcat(GPIO_AF_SPI, I2S_N)

#if defined STM32F2XX || defined STM32F4XX
  #define I2S_WS_GPIO_CLOCK       xcat(RCC_AHB1Periph_GPIO, I2S_WS_GPIO_N)
  #define I2S_MCK_GPIO_CLOCK      xcat(RCC_AHB1Periph_GPIO, I2S_MCK_GPIO_N)
  #define I2S_SCK_GPIO_CLOCK      xcat(RCC_AHB1Periph_GPIO, I2S_SCK_GPIO_N)
  #define I2S_SD_GPIO_CLOCK       xcat(RCC_AHB1Periph_GPIO, I2S_SD_GPIO_N)
  #define RCC_GPIOPeriphClockCmd  RCC_AHB1PeriphClockCmd
#else
  #error I2C module is not implemented for this line of STM32.
#endif

#define I2S_GPIO_CLOCK  (I2S_WS_GPIO_CLOCK  | I2S_MCK_GPIO_CLOCK | \
                         I2S_SCK_GPIO_CLOCK | I2S_SD_GPIO_CLOCK)

#define I2S_DMA_CLOCK       xcat(RCC_AHB1Periph_DMA, I2S_DMA_N)
#define I2S_DMA_CHANNEL     xcat(DMA_Channel_, I2S_DMA_CHANNEL_N)
#define I2S_DMA_STREAM      xcat4(DMA, I2S_DMA_N, _Stream, I2S_DMA_STREAM_N)
#define I2S_DMA_IRQ         xcat5(DMA, I2S_DMA_N, _Stream, I2S_DMA_STREAM_N, _IRQn)
#define I2S_DMA_IRQHandler  xcat5(DMA, I2S_DMA_N, _Stream, I2S_DMA_STREAM_N, _IRQHandler)
#define I2S_DMA_FLAG_TC     xcat(DMA_FLAG_TCIF, I2S_DMA_STREAM_N)
#define I2S_DMA_FLAG_HT     xcat(DMA_FLAG_HTIF, I2S_DMA_STREAM_N)
#define I2S_DMA_FLAG_FE     xcat(DMA_FLAG_FEIF, I2S_DMA_STREAM_N)
#define I2S_DMA_FLAG_TE     xcat(DMA_FLAG_TEIF, I2S_DMA_STREAM_N)
#define I2S_DMA_FLAG_DME    xcat(DMA_FLAG_DMEIF, I2S_DMA_STREAM_N)

#define I2S_DMA_FLAG_ALL    (I2S_DMA_FLAG_TC | I2S_DMA_FLAG_HT | \
                             I2S_DMA_FLAG_FE | I2S_DMA_FLAG_TE | \
                             I2S_DMA_FLAG_DME)

/** Internals **/

/* The maximum number of iterations for I2S waiting loops */
#define I2S_TIMEOUT  200000

/* Callback function called in interrupt routine. */
static void (* TransferCompleteCallback)(int16_t **, uint32_t *) = 0;

/* Global DMA configuration, initialized in I2Sconfigure and used
   later in interrupt routine */
static DMA_InitTypeDef DMA_InitStruct;

/** Public interface **/

/* Configure I2S interface in stereo mode, 16-bit per sample.
   Set sampling frequency. */
void I2Sconfigure(uint32_t frequency) {
  GPIO_InitTypeDef GPIO_InitStruct;
  I2S_InitTypeDef  I2S_InitStruct;

  RCC_APB1PeriphClockCmd(I2S_CLOCK, ENABLE);
  RCC_GPIOPeriphClockCmd(I2S_GPIO_CLOCK, ENABLE);

  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;

  GPIO_InitStruct.GPIO_Pin = I2S_MCK_PIN;
  GPIO_Init(I2S_MCK_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.GPIO_Pin = I2S_WS_PIN;
  GPIO_Init(I2S_WS_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.GPIO_Pin = I2S_SCK_PIN;
  GPIO_Init(I2S_SCK_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.GPIO_Pin = I2S_SD_PIN;
  GPIO_Init(I2S_SD_PORT, &GPIO_InitStruct);

  GPIO_PinAFConfig(I2S_WS_PORT,  I2S_WS_SOURCE,  I2S_AF);
  GPIO_PinAFConfig(I2S_SCK_PORT, I2S_SCK_SOURCE, I2S_AF);
  GPIO_PinAFConfig(I2S_SD_PORT,  I2S_SD_SOURCE,  I2S_AF);
  GPIO_PinAFConfig(I2S_MCK_PORT, I2S_MCK_SOURCE, I2S_AF);

  I2S_InitStruct.I2S_AudioFreq = frequency;
  I2S_InitStruct.I2S_Standard = I2S_Standard_Phillips;
  I2S_InitStruct.I2S_DataFormat = I2S_DataFormat_16b;
  I2S_InitStruct.I2S_CPOL = I2S_CPOL_Low;
  I2S_InitStruct.I2S_Mode = I2S_Mode_MasterTx;
  I2S_InitStruct.I2S_MCLKOutput = I2S_MCLKOutput_Enable;

  /* NOTE: The following function cannot correctly compute MCLK
     frequency for all combinations of PLLs setting and audio sampling
     frequency. */
  I2S_Init(I2S, &I2S_InitStruct);
  I2S_Cmd(I2S, ENABLE);
}

int I2Splay(int16_t const *sampleAddress, uint32_t sampleCount,
            i2s_play_t play, i2s_irq_source_t irq_source,
            unsigned prio, unsigned subprio,
            void (* callback)(int16_t **, uint32_t *)) {
  NVIC_InitTypeDef NVIC_InitStruct;
  int              timeout;

  TransferCompleteCallback = callback;

  RCC_AHB1PeriphClockCmd(I2S_DMA_CLOCK, ENABLE);

  /* Disable the I2S DMA stream and clear interrupt flags. */
  DMA_Cmd(I2S_DMA_STREAM, DISABLE);
  DMA_ClearFlag(I2S_DMA_STREAM, I2S_DMA_FLAG_ALL);

  /* Only initialize the structure. DMA_Init is called later
     and in I2S_DMA_IRQHandler. */
  DMA_InitStruct.DMA_Channel = I2S_DMA_CHANNEL;
  DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&I2S->DR;
  DMA_InitStruct.DMA_Memory0BaseAddr = (uint32_t)sampleAddress;
  DMA_InitStruct.DMA_DIR = DMA_DIR_MemoryToPeripheral;
  DMA_InitStruct.DMA_BufferSize = sampleCount;
  DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
  if (play == I2S_PLAY_FOREVER)
    DMA_InitStruct.DMA_Mode = DMA_Mode_Circular;
  else
    DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStruct.DMA_Priority = DMA_Priority_High;
  DMA_InitStruct.DMA_FIFOMode = DMA_FIFOMode_Enable;
  DMA_InitStruct.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
  DMA_InitStruct.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStruct.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;

  if (irq_source == I2S_IRQ_TRANSFER_COMPLETE) {
    /* Configure DMA IRQ. */
    DMA_ITConfig(I2S_DMA_STREAM, DMA_IT_TC, ENABLE);
    NVIC_InitStruct.NVIC_IRQChannel = I2S_DMA_IRQ;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = prio;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = subprio;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
  }

  /* Check if the DMA stream is effectively disabled. */
  timeout = I2S_TIMEOUT;
  while (DMA_GetCmdStatus(I2S_DMA_STREAM) != DISABLE)
    if (--timeout < 0)
      return -1;

  /* Configure the DMA stream with the new parameters and enable it. */
  DMA_Init(I2S_DMA_STREAM, &DMA_InitStruct);
  SPI_I2S_DMACmd(I2S, SPI_I2S_DMAReq_Tx, ENABLE);
  DMA_Cmd(I2S_DMA_STREAM, ENABLE);

  return 0;
}

void I2S_DMA_IRQHandler(void) {
  if (DMA_GetFlagStatus(I2S_DMA_STREAM, I2S_DMA_FLAG_TC)) {
    /* Transfer complete interrupt */
    DMA_ClearFlag(I2S_DMA_STREAM, I2S_DMA_FLAG_TC);
    if (DMA_InitStruct.DMA_Mode == DMA_Mode_Normal) {
      int16_t  *samples;
      uint32_t count;
      int      timeout;

      TransferCompleteCallback(&samples, &count);
      DMA_InitStruct.DMA_Memory0BaseAddr = (uint32_t)samples;
      DMA_InitStruct.DMA_BufferSize = count;

      /* Check if the DMA stream is effectively disabled. */
      timeout = I2S_TIMEOUT;
      while (DMA_GetCmdStatus(I2S_DMA_STREAM) != DISABLE)
        if (--timeout < 0)
          return; /* What to do? */

      DMA_Init(I2S_DMA_STREAM, &DMA_InitStruct);
      DMA_Cmd(I2S_DMA_STREAM, ENABLE);
    }
  }
  #if 0
  else if (DMA_GetFlagStatus(I2S_DMA_STREAM, I2S_DMA_FLAG_HT)) {
    /* Half Transfer complete interrupt */
    DMA_ClearFlag(I2S_DMA_STREAM, I2S_DMA_FLAG_HT);
  }
  else if (DMA_GetFlagStatus(I2S_DMA_STREAM, I2S_DMA_FLAG_TE) ||
           DMA_GetFlagStatus(I2S_DMA_STREAM, I2S_DMA_FLAG_FE) ||
           DMA_GetFlagStatus(I2S_DMA_STREAM, I2S_DMA_FLAG_DME)) {
    /* FIFO error interrupt */
    DMA_ClearFlag(I2S_DMA_STREAM,
                  I2S_DMA_FLAG_TE | I2S_DMA_FLAG_FE | I2S_DMA_FLAG_DME);
  }
  #endif
}

void I2Spause(void) {
  SPI_I2S_DMACmd(I2S, SPI_I2S_DMAReq_Tx, DISABLE);
  DMA_Cmd(I2S_DMA_STREAM, DISABLE);
}

void I2Sresume(void) {
  SPI_I2S_DMACmd(I2S, SPI_I2S_DMAReq_Tx, ENABLE);
  DMA_Cmd(I2S_DMA_STREAM, ENABLE);
}
