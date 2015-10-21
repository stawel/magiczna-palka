/**
  ******************************************************************************
  * @file    stm32f4_discovery_lis302dl.c
  * @author  MCD Application Team, Modified by MP
  * @version V1.1.0
  * @date    28-October-2011, 25-April-2012
  * @brief   This file provides a set of functions needed to manage the LIS302DL
  *          MEMS accelerometer available on STM32F4-Discovery Kit.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

#include <lis302dl.h>
#include <stm32f4xx_gpio.h>
#include <stm32f4xx_rcc.h>
#include <stm32f4xx_spi.h>

/** LIS302DL SPI pins in STM32F4-DISCOVERY board **/

#define LIS302DL_SPI                        SPI1
#define LIS302DL_SPI_CLK                    RCC_APB2Periph_SPI1

#define LIS302DL_SPI_SCK_PIN                GPIO_Pin_5
#define LIS302DL_SPI_SCK_GPIO_PORT          GPIOA
#define LIS302DL_SPI_SCK_GPIO_CLK           RCC_AHB1Periph_GPIOA
#define LIS302DL_SPI_SCK_SOURCE             GPIO_PinSource5
#define LIS302DL_SPI_SCK_AF                 GPIO_AF_SPI1

#define LIS302DL_SPI_MISO_PIN               GPIO_Pin_6
#define LIS302DL_SPI_MISO_GPIO_PORT         GPIOA
#define LIS302DL_SPI_MISO_GPIO_CLK          RCC_AHB1Periph_GPIOA
#define LIS302DL_SPI_MISO_SOURCE            GPIO_PinSource6
#define LIS302DL_SPI_MISO_AF                GPIO_AF_SPI1

#define LIS302DL_SPI_MOSI_PIN               GPIO_Pin_7
#define LIS302DL_SPI_MOSI_GPIO_PORT         GPIOA
#define LIS302DL_SPI_MOSI_GPIO_CLK          RCC_AHB1Periph_GPIOA
#define LIS302DL_SPI_MOSI_SOURCE            GPIO_PinSource7
#define LIS302DL_SPI_MOSI_AF                GPIO_AF_SPI1

#define LIS302DL_SPI_CS_PIN                 GPIO_Pin_3
#define LIS302DL_SPI_CS_GPIO_PORT           GPIOE
#define LIS302DL_SPI_CS_GPIO_CLK            RCC_AHB1Periph_GPIOE

#define LIS302DL_SPI_INT1_PIN               GPIO_Pin_0
#define LIS302DL_SPI_INT1_GPIO_PORT         GPIOE
#define LIS302DL_SPI_INT1_GPIO_CLK          RCC_AHB1Periph_GPIOE
#define LIS302DL_SPI_INT1_EXTI_LINE         EXTI_Line0
#define LIS302DL_SPI_INT1_EXTI_PORT_SOURCE  EXTI_PortSourceGPIOE
#define LIS302DL_SPI_INT1_EXTI_PIN_SOURCE   EXTI_PinSource0
#define LIS302DL_SPI_INT1_EXTI_IRQn         EXTI0_IRQn

#define LIS302DL_SPI_INT2_PIN               GPIO_Pin_1
#define LIS302DL_SPI_INT2_GPIO_PORT         GPIOE
#define LIS302DL_SPI_INT2_GPIO_CLK          RCC_AHB1Periph_GPIOE
#define LIS302DL_SPI_INT2_EXTI_LINE         EXTI_Line1
#define LIS302DL_SPI_INT2_EXTI_PORT_SOURCE  EXTI_PortSourceGPIOE
#define LIS302DL_SPI_INT2_EXTI_PIN_SOURCE   EXTI_PinSource1
#define LIS302DL_SPI_INT2_EXTI_IRQn         EXTI1_IRQn

/* Switch to SPI interface. */
static inline void LIS302DL_ChipSelect(void) {
  GPIO_ResetBits(LIS302DL_SPI_CS_GPIO_PORT, LIS302DL_SPI_CS_PIN);
}

/* Switch to I2C interface (switch SPI off). */
static inline void LIS302DL_ChipUnselect(void) {
  GPIO_SetBits(LIS302DL_SPI_CS_GPIO_PORT, LIS302DL_SPI_CS_PIN);
}

/** LIS302DL register addresses **/

#define LIS302DL_WHO_AM_I_ADDR             0x0F
#define LIS302DL_CTRL_REG1_ADDR            0x20
#define LIS302DL_CTRL_REG2_ADDR            0x21
#define LIS302DL_CTRL_REG3_ADDR            0x22
#define LIS302DL_HP_FILTER_RESET_REG_ADDR  0x23
#define LIS302DL_STATUS_REG_ADDR           0x27
#define LIS302DL_OUT_X_ADDR                0x29
#define LIS302DL_OUT_Y_ADDR                0x2B
#define LIS302DL_OUT_Z_ADDR                0x2D
#define LIS302DL_FF_WU_CFG1_REG_ADDR       0x30
#define LIS302DL_FF_WU_SRC1_REG_ADDR       0x31
#define LIS302DL_FF_WU_THS1_REG_ADDR       0x32
#define LIS302DL_FF_WU_DURATION1_REG_ADDR  0x33
#define LIS302DL_FF_WU_CFG2_REG_ADDR       0x34
#define LIS302DL_FF_WU_SRC2_REG_ADDR       0x35
#define LIS302DL_FF_WU_THS2_REG_ADDR       0x36
#define LIS302DL_FF_WU_DURATION2_REG_ADDR  0x37
#define LIS302DL_CLICK_CFG_REG_ADDR        0x38
#define LIS302DL_CLICK_SRC_REG_ADDR        0x39
#define LIS302DL_CLICK_THSY_X_REG_ADDR     0x3B
#define LIS302DL_CLICK_THSZ_REG_ADDR       0x3C
#define LIS302DL_CLICK_TIMELIMIT_REG_ADDR  0x3D
#define LIS302DL_CLICK_LATENCY_REG_ADDR    0x3E
#define LIS302DL_CLICK_WINDOW_REG_ADDR     0x3F

static void LIS302DL_LowLevelInit(void);
static void LIS302DL_Write(uint8_t *, uint8_t, uint32_t);
static void LIS302DL_Read(uint8_t *, uint8_t, uint32_t);
static uint8_t LIS302DL_SPI_ReadWriteByte(uint8_t);
static uint8_t LIS302DL_SPI_Error(void);

/** Module public interface **/

/**
  * @brief  LIS302DL initialization
  * @param  LIS302DL_Config_Struct: pointer to a LIS302DL_Config_TypeDef
  *         structure that contains the configuration setting for the LIS302DL.
  * @retval None
  */
void LIS302DL_Init(LIS302DL_InitTypeDef *LIS302DL_InitStruct) {
  uint8_t ctrl;

  LIS302DL_LowLevelInit();

  ctrl = LIS302DL_InitStruct->Output_DataRate |
         LIS302DL_InitStruct->Power_Mode |
         LIS302DL_InitStruct->Full_Scale |
         LIS302DL_InitStruct->Self_Test |
         LIS302DL_InitStruct->Axes_Enable;

  LIS302DL_Write(&ctrl, LIS302DL_CTRL_REG1_ADDR, 1);
}

/**
  * @brief  LIS302DL internal high pass filter configuration
  * @param  LIS302DL_Filter_ConfigTypeDef: pointer to a LIS302DL_FilterConfig_TypeDef
  *         structure that contains the configuration setting for the LIS302DL filter.
  * @retval None
  */
void LIS302DL_FilterConfig(LIS302DL_FilterConfigTypeDef *LIS302DL_FilterConfigStruct) {
  uint8_t ctrl;

  LIS302DL_Read(&ctrl, LIS302DL_CTRL_REG2_ADDR, 1);
  ctrl &= ~(LIS302DL_FILTEREDDATASELECTION_OUTPUTREGISTER |
            LIS302DL_HIGHPASSFILTER_LEVEL_3 |
            LIS302DL_HIGHPASSFILTERINTERRUPT_1_2);
  ctrl |= LIS302DL_FilterConfigStruct->HighPassFilter_Data_Selection |
          LIS302DL_FilterConfigStruct->HighPassFilter_CutOff_Frequency |
          LIS302DL_FilterConfigStruct->HighPassFilter_Interrupt;
  LIS302DL_Write(&ctrl, LIS302DL_CTRL_REG2_ADDR, 1);
}

/**
  * @brief  LIS302DL interrupt configuration
  * @param  LIS302DL_InterruptConfig_TypeDef: pointer to a LIS302DL_InterruptConfig_TypeDef
  *         structure that contains the configuration setting for the LIS302DL interrupt.
  * @retval None
  */
void LIS302DL_InterruptConfig(LIS302DL_InterruptConfigTypeDef *LIS302DL_IntConfigStruct) {
  uint8_t ctrl;

  LIS302DL_Read(&ctrl, LIS302DL_CLICK_CFG_REG_ADDR, 1);
  ctrl = LIS302DL_IntConfigStruct->Latch_Request|
         LIS302DL_IntConfigStruct->DoubleClick_Axes |
         LIS302DL_IntConfigStruct->SingleClick_Axes;
  LIS302DL_Write(&ctrl, LIS302DL_CLICK_CFG_REG_ADDR, 1);
}

/**
  * @brief  Data rate command for LIS302DL
  * @param  DataRateValue: Data rate value
  *   This parameter can be one of the following values:
  *     @arg LIS302DL_DATARATE_100: 100 Hz output data rate
  *     @arg LIS302DL_DATARATE_400: 400 Hz output data rate
  * @retval None
  */
void LIS302DL_DataRateCmd(uint8_t DataRateValue) {
  uint8_t tmpreg;

  LIS302DL_Read(&tmpreg, LIS302DL_CTRL_REG1_ADDR, 1);
  tmpreg &= ~LIS302DL_DATARATE_400;
  tmpreg |= DataRateValue;
  LIS302DL_Write(&tmpreg, LIS302DL_CTRL_REG1_ADDR, 1);
}

/**
  * @brief  Change the low power mode for LIS302DL
  * @param  LowPowerMode: new state for the lowpower mode
  *   This parameter can be one of the following values:
  *     @arg LIS302DL_LOWPOWERMODE_POWERDOWN: power down mode
  *     @arg LIS302DL_LOWPOWERMODE_ACTIVE: active mode
  * @retval None
  */
void LIS302DL_LowPowerCmd(uint8_t LowPowerMode) {
  uint8_t tmpreg;

  LIS302DL_Read(&tmpreg, LIS302DL_CTRL_REG1_ADDR, 1);
  tmpreg &= ~LIS302DL_LOWPOWERMODE_ACTIVE;
  tmpreg |= LowPowerMode;
  LIS302DL_Write(&tmpreg, LIS302DL_CTRL_REG1_ADDR, 1);
}

/**
  * @brief  Change the full scale of LIS302DL
  * @param  FS_value: new full scale value
  *   This parameter can be one of the following values:
  *     @arg LIS302DL_FULLSCALE_2_3: +-2.3g
  *     @arg LIS302DL_FULLSCALE_9_2: +-9.2g
  * @retval None
  */
void LIS302DL_FullScaleCmd(uint8_t FS_value) {
  uint8_t tmpreg;

  LIS302DL_Read(&tmpreg, LIS302DL_CTRL_REG1_ADDR, 1);
  tmpreg &= ~LIS302DL_FULLSCALE_9_2;
  tmpreg |= FS_value;
  LIS302DL_Write(&tmpreg, LIS302DL_CTRL_REG1_ADDR, 1);
}

#define LIS302DL_BOOT_NORMALMODE    0x00
#define LIS302DL_BOOT_REBOOTMEMORY  0x40

/**
  * @brief  Reboot memory content of LIS302DL
  * @param  None
  * @retval None
  */
void LIS302DL_RebootCmd(void) {
  uint8_t tmpreg;

  LIS302DL_Read(&tmpreg, LIS302DL_CTRL_REG2_ADDR, 1);
  tmpreg |= LIS302DL_BOOT_REBOOTMEMORY;
  LIS302DL_Write(&tmpreg, LIS302DL_CTRL_REG2_ADDR, 1);
}

#define LIS302DL_SENSITIVITY_2_3G  18
#define LIS302DL_SENSITIVITY_9_2G  72

/**
  * @brief  Read LIS302DL output registers and calculate the acceleration
  * @param  out: pointer to a buffer to store data
  * @retval None
  */
void LIS302DL_ReadACC(int32_t *out) {
  int     i;
  uint8_t crtl;
  int8_t  buffer[3];

  LIS302DL_Read(&crtl, LIS302DL_CTRL_REG1_ADDR, 1);
  LIS302DL_ReadXYZ(buffer, 3);

  if ((crtl & 0x20) == 0x00) {
    /* FS == 0, hence the typical sensitivity value is 18 mg/digit */
    for (i = 0; i < 3; ++i)
      *out++ = LIS302DL_SENSITIVITY_2_3G * (int32_t)buffer[i];
  }
  else {
    /* FS == 1, hence the typical sensitivity value is 72 mg/digit */
    for (i = 0; i < 3; ++i)
      *out++ = LIS302DL_SENSITIVITY_9_2G * (int32_t)buffer[i];
  }
}

/**
  * @brief  Reads XYZ block of data from the LIS302DL
  * @param  buffer: pointer to the buffer that receives the data read
  * @param  count: number of bytes to read
  * @retval None
  */
void LIS302DL_ReadXYZ(int8_t *buffer, uint32_t count) {
  uint32_t i;
  uint8_t  tmp_buffer[6];

  LIS302DL_Read(tmp_buffer, LIS302DL_OUT_X_ADDR, (count << 1) - 1);
  for (i = 0; i < count; ++i)
    buffer[i] = tmp_buffer[i << 1];
}

/** Internal functions **/

/**
  * @brief  Initializes the low level interface used to drive the LIS302DL
  * @param  None
  * @retval None
  */
void LIS302DL_LowLevelInit() {
  GPIO_InitTypeDef GPIO_InitStruct;
  SPI_InitTypeDef  SPI_InitStruct;

  RCC_APB2PeriphClockCmd(LIS302DL_SPI_CLK, ENABLE);
  RCC_AHB1PeriphClockCmd(LIS302DL_SPI_SCK_GPIO_CLK |
                         LIS302DL_SPI_MISO_GPIO_CLK |
                         LIS302DL_SPI_MOSI_GPIO_CLK |
                         LIS302DL_SPI_CS_GPIO_CLK |
                         LIS302DL_SPI_INT1_GPIO_CLK |
                         LIS302DL_SPI_INT2_GPIO_CLK,
                         ENABLE);

  GPIO_PinAFConfig(LIS302DL_SPI_SCK_GPIO_PORT,  LIS302DL_SPI_SCK_SOURCE,  LIS302DL_SPI_SCK_AF);
  GPIO_PinAFConfig(LIS302DL_SPI_MISO_GPIO_PORT, LIS302DL_SPI_MISO_SOURCE, LIS302DL_SPI_MISO_AF);
  GPIO_PinAFConfig(LIS302DL_SPI_MOSI_GPIO_PORT, LIS302DL_SPI_MOSI_SOURCE, LIS302DL_SPI_MOSI_AF);

  GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  /* TODO: Why is the pull-down resistor configured here?
  GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_DOWN; */
  GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;

  GPIO_InitStruct.GPIO_Pin = LIS302DL_SPI_SCK_PIN;
  GPIO_Init(LIS302DL_SPI_SCK_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.GPIO_Pin = LIS302DL_SPI_MOSI_PIN;
  GPIO_Init(LIS302DL_SPI_MOSI_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.GPIO_Pin = LIS302DL_SPI_MISO_PIN;
  GPIO_Init(LIS302DL_SPI_MISO_GPIO_PORT, &GPIO_InitStruct);

  LIS302DL_ChipUnselect();

  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;

  GPIO_InitStruct.GPIO_Pin = LIS302DL_SPI_CS_PIN;
  GPIO_Init(LIS302DL_SPI_CS_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;

  GPIO_InitStruct.GPIO_Pin = LIS302DL_SPI_INT1_PIN;
  GPIO_Init(LIS302DL_SPI_INT1_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.GPIO_Pin = LIS302DL_SPI_INT2_PIN;
  GPIO_Init(LIS302DL_SPI_INT2_GPIO_PORT, &GPIO_InitStruct);

  SPI_I2S_DeInit(LIS302DL_SPI);
  SPI_InitStruct.SPI_Direction         = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStruct.SPI_DataSize          = SPI_DataSize_8b;
  SPI_InitStruct.SPI_CPOL              = SPI_CPOL_Low;
  SPI_InitStruct.SPI_CPHA              = SPI_CPHA_1Edge;
  SPI_InitStruct.SPI_NSS               = SPI_NSS_Soft;
  SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
  SPI_InitStruct.SPI_FirstBit          = SPI_FirstBit_MSB;
  SPI_InitStruct.SPI_CRCPolynomial     = 7;
  SPI_InitStruct.SPI_Mode              = SPI_Mode_Master;
  SPI_Init(LIS302DL_SPI, &SPI_InitStruct);

  SPI_Cmd(LIS302DL_SPI, ENABLE);
}

#define MULTIPLEBYTE_CMD  0x40
#define READWRITE_CMD     0x80
#define DUMMY_BYTE        0x00

/**
  * @brief  Write one byte to the LIS302DL
  * @param  pBuffer: pointer to the buffer containing the data to be written
  * @param  WriteAddr: LIS302DL's internal address to write
  * @param  NumByteToWrite: the number of bytes to write
  * @retval None
  */
void LIS302DL_Write(uint8_t *pBuffer, uint8_t WriteAddr, uint32_t NumByteToWrite) {
  /* Configure the MS bit:
      - When 0, the address will remain unchanged in multiple read/write commands.
      - When 1, the address will be auto incremented in multiple read/write commands. */
  if (NumByteToWrite > 1)
    WriteAddr |= MULTIPLEBYTE_CMD;

  LIS302DL_ChipSelect();
  LIS302DL_SPI_ReadWriteByte(WriteAddr);
  while (NumByteToWrite-- > 0)
    LIS302DL_SPI_ReadWriteByte(*pBuffer++);
  LIS302DL_ChipUnselect();
}

/**
  * @brief  Read a block of data from the LIS302DL
  * @param  pBuffer: pointer to the buffer that receives the data read
  * @param  ReadAddr: LIS302DL's internal address to read
  * @param  NumByteToRead: number of bytes to read
  * @retval None
  */
void LIS302DL_Read(uint8_t *pBuffer, uint8_t ReadAddr, uint32_t NumByteToRead) {
  if (NumByteToRead > 1)
    ReadAddr |= READWRITE_CMD | MULTIPLEBYTE_CMD;
  else
    ReadAddr |= READWRITE_CMD;

  LIS302DL_ChipSelect();
  LIS302DL_SPI_ReadWriteByte(ReadAddr);
  while (NumByteToRead-- > 0)
    *pBuffer++ = LIS302DL_SPI_ReadWriteByte(DUMMY_BYTE);
  LIS302DL_ChipUnselect();
}

#define LIS302DL_TIMEOUT  0x1000

/**
  * @brief  Send a byte through the SPI interface and return the byte received
  *         from the SPI bus
  * @param  Byte: byte to send
  * @retval The received byte value
  */
uint8_t LIS302DL_SPI_ReadWriteByte(uint8_t byte) {
  uint32_t LIS302DLTimeout;

  /* Loop while DR register in not emplty. */
  LIS302DLTimeout = LIS302DL_TIMEOUT;
  while (SPI_GetFlagStatus(LIS302DL_SPI, SPI_I2S_FLAG_TXE) == RESET)
    if (--LIS302DLTimeout == 0)
      return LIS302DL_SPI_Error();

  /* Send a byte through the SPI peripheral */
  SPI_SendData(LIS302DL_SPI, byte);

  /* Wait to receive a byte. */
  LIS302DLTimeout = LIS302DL_TIMEOUT;
  while (SPI_GetFlagStatus(LIS302DL_SPI, SPI_I2S_FLAG_RXNE) == RESET)
    if (--LIS302DLTimeout == 0)
      return LIS302DL_SPI_Error();

  return (uint8_t)SPI_ReceiveData(LIS302DL_SPI);
}

/**
  * @brief  Basic management of the timeout situation
  * @param  None
  * @retval The value to be returned by LIS302DL_SPI_ReadWriteByte
  */
uint8_t LIS302DL_SPI_Error() {
  return DUMMY_BYTE;
}

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
