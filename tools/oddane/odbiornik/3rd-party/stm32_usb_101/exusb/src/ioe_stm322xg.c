#include <delay.h>
#include <i2c.h>
#include <i2s.h>
#include <ioe_stm322xg.h>

/** Hardware configuration **/

/* The audio chip reset pin is connected to the second IO expander. */
#define AUDIO_RESET_PIN  IO_Pin_2

/** I2C constants **/

/* I2C chips adresses and IDs */
#define IOE_1_ADDR       0x82
#define IOE_2_ADDR       0x88
#define STMPE811_ID      0x0811
#define CS43L22_ADDRESS  0x94
#define CS43L22_ID       0xE0
#define CS43L22_ID_MASK  0xF8

/* Delays */
#define T20ms  600000

/** STMPE811 IO expander definitions **/

/* Identification registers */
#define IOE_REG_CHP_ID             0x00 /* 16-bit register */
#define IOE_REG_ID_VER             0x02

/* General control registers */
#define IOE_REG_SYS_CTRL1          0x03
#define IOE_REG_SYS_CTRL2          0x04
#define IOE_REG_SPI_CFG            0x08

/* Interrupt control register */
#define IOE_REG_INT_CTRL           0x09
#define IOE_REG_INT_EN             0x0A
#define IOE_REG_INT_STA            0x0B
#define IOE_REG_GPIO_INT_EN        0x0C
#define IOE_REG_GPIO_INT_STA       0x0D

/* GPIO registers */
#define IOE_REG_GPIO_SET_PIN       0x10
#define IOE_REG_GPIO_CLR_PIN       0x11
#define IOE_REG_GPIO_MP_STA        0x12
#define IOE_REG_GPIO_DIR           0x13
#define IOE_REG_GPIO_ED            0x14
#define IOE_REG_GPIO_RE            0x15
#define IOE_REG_GPIO_FE            0x16
#define IOE_REG_GPIO_AF            0x17

/* ADC Registers */
#define IOE_REG_ADC_INT_EN         0x0E
#define IOE_REG_ADC_INT_STA        0x0F
#define IOE_REG_ADC_CTRL1          0x20
#define IOE_REG_ADC_CTRL2          0x21
#define IOE_REG_ADC_CAPT           0x22
#define IOE_REG_ADC_DATA_CH0       0x30 /* 16-bit register */
#define IOE_REG_ADC_DATA_CH1       0x32 /* 16-bit register */
#define IOE_REG_ADC_DATA_CH2       0x34 /* 16-bit register */
#define IOE_REG_ADC_DATA_CH3       0x36 /* 16-bit register */
#define IOE_REG_ADC_DATA_CH4       0x38 /* 16-bit register */
#define IOE_REG_ADC_DATA_CH5       0x3A /* 16-bit register */
#define IOE_REG_ADC_DATA_CH6       0x3B /* 16-bit register */
#define IOE_REG_ADC_DATA_CH7       0x3C /* 16-bit register */

/* Touch screen registers */
#define IOE_REG_TSC_CTRL           0x40
#define IOE_REG_TSC_CFG            0x41
#define IOE_REG_WDM_TR_X           0x42
#define IOE_REG_WDM_TR_Y           0x44
#define IOE_REG_WDM_BL_X           0x46
#define IOE_REG_WDM_BL_Y           0x48
#define IOE_REG_FIFO_TH            0x4A
#define IOE_REG_FIFO_STA           0x4B
#define IOE_REG_FIFO_SIZE          0x4C
#define IOE_REG_TSC_DATA_X         0x4D
#define IOE_REG_TSC_DATA_Y         0x4F
#define IOE_REG_TSC_DATA_Z         0x51
#define IOE_REG_TSC_DATA_XYZ       0x52
#define IOE_REG_TSC_FRACT_XYZ      0x56
#define IOE_REG_TSC_DATA           0x57
#define IOE_REG_TSC_I_DRIVE        0x58
#define IOE_REG_TSC_SHIELD         0x59

/* Temperature sensor registers */
#define IOE_REG_TEMP_CTRL          0x60
#define IOE_REG_TEMP_DATA          0x61
#define IOE_REG_TEMP_TH            0x62

/* IO expander functionalities definitions */
#define IOE_ADC_FCT       0x01
#define IOE_TS_FCT        0x02
#define IOE_IO_FCT        0x04
#define IOE_TEMPSENS_FCT  0x08

/* IO pin direction */
#define DIRECTION_IN   0x00
#define DIRECTION_OUT  0x01

/* Edge detection value */
#define EDGE_FALLING  0x01
#define EDGE_RISING   0x02

/** Private types and functions **/

typedef enum {
  DISABLE = 0,
  ENABLE = 1
} state_t;

static uint16_t IOEreadID(uint8_t);
static int      IOEreset(uint8_t);
static int      IOEfnctCmd(uint8_t, uint8_t, state_t);
static int      IOEpinConfig(uint8_t, uint8_t, uint8_t);
static int      IOEafConfig(uint8_t, uint8_t, state_t);
static int      IOEedgeConfig(uint8_t, uint8_t, uint8_t);
static int      IOEwritePin(uint8_t, uint8_t, uint8_t);

/** Public interface **/

/* Initializes and configures the second IO expander functionalities.
   Returns 0 if all initializations done correctly, and a negative
   value if an error. */
int IOE2configure() {
  /* Check if device is operational. */
  if (IOEreadID(IOE_2_ADDR) != STMPE811_ID)
    return -1;

  /* Generate IO expander software reset */
  if (IOEreset(IOE_2_ADDR) < 0)
    return -1;

  /* Enable the GPIO functionalities */
  if (IOEfnctCmd(IOE_2_ADDR, IOE_IO_FCT, ENABLE) < 0)
    return -1;

  /* Configure the audio chip reset pin in the output mode. */
  if (IOEpinConfig(IOE_2_ADDR, AUDIO_RESET_PIN, DIRECTION_OUT) < 0)
    return -1;
  if (IOEafConfig(IOE_2_ADDR, AUDIO_RESET_PIN, ENABLE) < 0)
    return -1;
  if (IOEwritePin(IOE_2_ADDR, AUDIO_RESET_PIN, 0) < 0)
    return -1;

  /* Configure the joystick pins in the input mode */
  if (IOEpinConfig(IOE_2_ADDR, JOY_IO_PINS, DIRECTION_IN) < 0)
    return -1;
  if (IOEafConfig(IOE_2_ADDR, JOY_IO_PINS, ENABLE) < 0)
    return -1;
  if (IOEedgeConfig(IOE_2_ADDR, JOY_IO_PINS, EDGE_FALLING | EDGE_RISING) < 0)
    return -1;

  return 0;
}

/* Control the reset pin of the audio chip. */
int IOE2audioReset(int bitvalue) {
  if (bitvalue)
    return I2CwriteDeviceRegister(IOE_2_ADDR, IOE_REG_GPIO_SET_PIN, AUDIO_RESET_PIN);
  else
    return I2CwriteDeviceRegister(IOE_2_ADDR, IOE_REG_GPIO_CLR_PIN, AUDIO_RESET_PIN);
}

/* Return the current joystick status. */
unsigned IOE2joystickGetState() {
  uint8_t value;

  if (I2CreadDeviceRegister(IOE_2_ADDR, IOE_REG_GPIO_MP_STA, &value) == 0)
    return ~value & JOY_IO_PINS;
  else
    return 0;
}

/** Private functions **/

/* Read the selected IO expande device's ID. */
uint16_t IOEreadID(uint8_t addr) {
  union {
    uint8_t  b[2];
    uint16_t w;
  } v;

  if (I2CreadDeviceRegister(addr, IOE_REG_CHP_ID, v.b + 1) == 0 &&
      I2CreadDeviceRegister(addr, IOE_REG_CHP_ID + 1, v.b) == 0)
    return v.w;
  else
    return 0;
}

/* Reset the IO expander by software (SYS_CTRL1, RESET bit). */
int IOEreset(uint8_t addr) {
  /* Power down the IO expander. */
  if (I2CwriteDeviceRegister(addr, IOE_REG_SYS_CTRL1, 0x02) < 0)
    return -1;

  /* Wait to ensure that registers are erased. */
  Delay(T20ms);

  /* Power on the IO expander. */
  return I2CwriteDeviceRegister(addr, IOE_REG_SYS_CTRL1, 0x00);
}

/* Configure the selected IO expander functionalities. */
int IOEfnctCmd(uint8_t addr, uint8_t fun, state_t state) {
  uint8_t value;

  if (I2CreadDeviceRegister(addr, IOE_REG_SYS_CTRL2, &value) < 0)
    return -1;
  if (state == ENABLE)
    value &= ~fun;
  else
    value |= fun;
  return I2CwriteDeviceRegister(addr, IOE_REG_SYS_CTRL2, value);
}

/* Configure the selected pins direction (input or output). */
int IOEpinConfig(uint8_t addr, uint8_t pins, uint8_t direction) {
  uint8_t value;

  if (I2CreadDeviceRegister(addr, IOE_REG_GPIO_DIR, &value) < 0)
    return -1;
  if (direction == DIRECTION_OUT)
    value |= pins;
  else
    value &= ~pins;
  return I2CwriteDeviceRegister(addr, IOE_REG_GPIO_DIR, value);
}

/* Configure the selected pins to be in alternate function or not. */
int IOEafConfig(uint8_t addr, uint8_t pins, state_t state) {
  uint8_t value;

  if (I2CreadDeviceRegister(addr, IOE_REG_GPIO_AF, &value) < 0)
    return -1;
  if (state == ENABLE)
    value |= pins;
  else
    value &= ~pins;
  return I2CwriteDeviceRegister(addr, IOE_REG_GPIO_AF, value);
}

/* Configure the edge for which a transition is detectable for the
   selected pins. */
int IOEedgeConfig(uint8_t addr, uint8_t pins, uint8_t edge) {
  uint8_t fe, re;

  if (I2CreadDeviceRegister(addr, IOE_REG_GPIO_FE, &fe) < 0)
    return -1;
  if (I2CreadDeviceRegister(addr, IOE_REG_GPIO_RE, &re) < 0)
    return -1;

  /* Disable the falling edge. */
  fe &= ~pins;
  /* Disable the rising edge. */
  re &= ~pins;

  /* Enable the falling edge if selected. */
  if (edge & EDGE_FALLING)
    fe |= pins;
  /* Enable the rising edge if selected. */
  if (edge & EDGE_RISING)
    re |= pins;

  if (I2CwriteDeviceRegister(addr, IOE_REG_GPIO_FE, fe) < 0)
    return -1;
  if (I2CwriteDeviceRegister(addr, IOE_REG_GPIO_RE, re) < 0)
    return -1;

  return 0;
}

/* Write a bit value to an output pin. */
int IOEwritePin(uint8_t addr, uint8_t pin, uint8_t bitvalue) {
  if (bitvalue)
    return I2CwriteDeviceRegister(addr, IOE_REG_GPIO_SET_PIN, pin);
  else
    return I2CwriteDeviceRegister(addr, IOE_REG_GPIO_CLR_PIN, pin);
}
