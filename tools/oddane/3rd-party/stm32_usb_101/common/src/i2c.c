#include <board_def.h>
#include <i2c.h>

#define I2C             xcat(I2C, I2C_N)
#define I2C_CLOCK       xcat(RCC_APB1Periph_I2C, I2C_N)
#define I2C_GPIO        xcat(GPIO, I2C_GPIO_N)
#define I2C_SCL_PIN     xcat(GPIO_Pin_, I2C_SCL_PIN_N)
#define I2C_SCL_SOURCE  xcat(GPIO_PinSource, I2C_SCL_PIN_N)
#define I2C_SDA_PIN     xcat(GPIO_Pin_, I2C_SDA_PIN_N)
#define I2C_SDA_SOURCE  xcat(GPIO_PinSource, I2C_SDA_PIN_N)
#define I2C_AF          xcat(GPIO_AF_I2C, I2C_N)

#if defined STM32F2XX || defined STM32F4XX
  #define I2C_GPIO_CLOCK          xcat(RCC_AHB1Periph_GPIO, I2C_GPIO_N)
  #define RCC_GPIOPeriphClockCmd  RCC_AHB1PeriphClockCmd
#elif defined STM32L1XX_MD || defined STM32L1XX_MDP || defined STM32L1XX_HD
  #define I2C_GPIO_CLOCK          xcat(RCC_AHBPeriph_GPIO, I2C_GPIO_N)
  #define RCC_GPIOPeriphClockCmd  RCC_AHBPeriphClockCmd
#else
  #error I2C module is not implemented for this STM32 line.
#endif

/* I2C master address */
#define I2C_OWN_ADDRESS  0x00

/* I2C clock speed in Hz */
#define I2C_SPEED  100000

/* The maximum number of iterations for I2C waiting loops */
#define I2C_TIMEOUT  25000

/* Initialize the GPIO pins used by the I2C bus and
   configure the I2C peripheral. */
void I2Cconfigure() {
  GPIO_InitTypeDef GPIO_InitStruct;
  I2C_InitTypeDef  I2C_InitStruct;

  RCC_APB1PeriphClockCmd(I2C_CLOCK, ENABLE);
  RCC_GPIOPeriphClockCmd(I2C_GPIO_CLOCK, ENABLE);

  GPIO_InitStruct.GPIO_Pin = I2C_SCL_PIN | I2C_SDA_PIN;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(I2C_GPIO, &GPIO_InitStruct);

  GPIO_PinAFConfig(I2C_GPIO, I2C_SCL_SOURCE, I2C_AF);
  GPIO_PinAFConfig(I2C_GPIO, I2C_SDA_SOURCE, I2C_AF);

  I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;
  I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;
  I2C_InitStruct.I2C_OwnAddress1 = I2C_OWN_ADDRESS;
  I2C_InitStruct.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_InitStruct.I2C_ClockSpeed = I2C_SPEED;
  I2C_Init(I2C, &I2C_InitStruct);
}

/* Write the I2C device register value. Return 0 on success, and
   a negative value if an error occurs. */
int I2CwriteDeviceRegister(uint8_t addr, uint8_t reg, uint8_t value) {
  int I2Ctimer;

  I2C_GenerateSTART(I2C, ENABLE);

  /* SB == start bit */
  I2Ctimer = I2C_TIMEOUT;
  while (I2C_GetFlagStatus(I2C, I2C_FLAG_SB) == RESET)
    if (--I2Ctimer < 0)
      return -1;

  I2C_Send7bitAddress(I2C, addr, I2C_Direction_Transmitter);

  I2Ctimer = I2C_TIMEOUT;
  while (!I2C_CheckEvent(I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    if (--I2Ctimer < 0)
      return -1;

  I2C_SendData(I2C, reg);

  /* TXE == data register empty, BTF == byte transfer finished */
  I2Ctimer = I2C_TIMEOUT;
  while (!I2C_GetFlagStatus(I2C, I2C_FLAG_TXE) &&
         !I2C_GetFlagStatus(I2C, I2C_FLAG_BTF))
    if (--I2Ctimer < 0)
      return -1;

  I2C_SendData(I2C, value);

  /* BTF == byte transfer finished */
  I2Ctimer = I2C_TIMEOUT;
  while (!I2C_GetFlagStatus(I2C, I2C_FLAG_BTF))
    if (--I2Ctimer < 0)
      return -1;

  I2C_GenerateSTOP(I2C, ENABLE);

  return 0;
}

/* Read the I2C device register value. Return 0 on success, and
   a negative value if an error occurs. */
int I2CreadDeviceRegister(uint8_t addr, uint8_t reg, uint8_t *value) {
  int I2Ctimer;

  /* Return the value 0, when an error occurs. */
  *value = 0;

  I2C_GenerateSTART(I2C, ENABLE);

  /* SB == start bit */
  I2Ctimer = I2C_TIMEOUT;
  while (!I2C_GetFlagStatus(I2C, I2C_FLAG_SB))
    if (--I2Ctimer < 0)
      return -1;

  I2C_Send7bitAddress(I2C, addr, I2C_Direction_Transmitter);

  I2Ctimer = I2C_TIMEOUT;
  while (!I2C_CheckEvent(I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    if (--I2Ctimer < 0)
      return -1;

  I2C_SendData(I2C, reg);

  /* TXE == data register empty, BTF == byte transfer finished */
  I2Ctimer = I2C_TIMEOUT;
  while (!I2C_GetFlagStatus(I2C, I2C_FLAG_TXE) &&
         !I2C_GetFlagStatus(I2C, I2C_FLAG_BTF))
    if (--I2Ctimer < 0)
      return -1;

  I2C_GenerateSTART(I2C, ENABLE);

  /* SB == start bit */
  I2Ctimer = I2C_TIMEOUT;
  while (!I2C_GetFlagStatus(I2C, I2C_FLAG_SB))
    if (--I2Ctimer < 0)
      return -1;

  I2C_Send7bitAddress(I2C, addr, I2C_Direction_Receiver);

  I2Ctimer = I2C_TIMEOUT;
  while (!I2C_CheckEvent(I2C, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
    if (--I2Ctimer < 0)
      return -1;

  /* Receive only one byte. Send NACK after the first received byte. */
  I2C_AcknowledgeConfig(I2C, DISABLE);

  I2Ctimer = I2C_TIMEOUT;
  while (!I2C_CheckEvent(I2C, I2C_EVENT_MASTER_BYTE_RECEIVED))
    if (--I2Ctimer < 0)
      return -1;

  I2C_GenerateSTOP(I2C, ENABLE);

  *value = I2C_ReceiveData(I2C);

  return 0;
}
