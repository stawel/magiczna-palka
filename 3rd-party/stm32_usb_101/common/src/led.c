#include <board_def.h>
#include <delay.h>
#include <led.h>

#define GPIO_RED_LED    xcat(GPIO, RED_LED_GPIO_N)
#define GPIO_GREEN_LED  xcat(GPIO, GREEN_LED_GPIO_N)
#define PIN_RED_LED     xcat(GPIO_Pin_, RED_LED_PIN_N)
#define PIN_GREEN_LED   xcat(GPIO_Pin_, GREEN_LED_PIN_N)

#if defined STM32F2XX || defined STM32F4XX
  #define RCC_RED_LED    xcat(RCC_AHB1Periph_GPIO, RED_LED_GPIO_N)
  #define RCC_GREEN_LED  xcat(RCC_AHB1Periph_GPIO, GREEN_LED_GPIO_N)
  #define GPIO_MODE_OUT  GPIO_Mode_OUT /* PP 2 MHz */
  #define RCC_PeriphClockCmd  RCC_AHB1PeriphClockCmd
#elif defined STM32F0XX || defined STM32F30X || defined STM32F37X || \
      defined STM32L1XX_MD || defined STM32L1XX_MDP || defined STM32L1XX_HD
  #define RCC_RED_LED    xcat(RCC_AHBPeriph_GPIO, RED_LED_GPIO_N)
  #define RCC_GREEN_LED  xcat(RCC_AHBPeriph_GPIO, GREEN_LED_GPIO_N)
  #define GPIO_MODE_OUT  GPIO_Mode_OUT /* PP, F0 fast speed, L1 400 kHz */
  #define RCC_PeriphClockCmd  RCC_AHBPeriphClockCmd
#else /* STM32F10x */
  #define RCC_RED_LED    xcat(RCC_APB2Periph_GPIO, RED_LED_GPIO_N)
  #define RCC_GREEN_LED  xcat(RCC_APB2Periph_GPIO, GREEN_LED_GPIO_N)
  #define GPIO_MODE_OUT  GPIO_Mode_Out_PP /* PP 2 MHz */
  #define RCC_PeriphClockCmd  RCC_APB2PeriphClockCmd
#endif

#define LED_OFF  (!(LED_ON))

void LEDconfigure(void) {
  GPIO_InitTypeDef GPIO_InitStruct;

  RCC_PeriphClockCmd(RCC_RED_LED | RCC_GREEN_LED, ENABLE);

  GPIO_WriteBit(GPIO_RED_LED, PIN_RED_LED, LED_OFF);
  GPIO_WriteBit(GPIO_GREEN_LED, PIN_GREEN_LED, LED_OFF);

  GPIO_StructInit(&GPIO_InitStruct);
  GPIO_InitStruct.GPIO_Mode = GPIO_MODE_OUT;

  GPIO_InitStruct.GPIO_Pin = PIN_RED_LED;
  GPIO_Init(GPIO_RED_LED, &GPIO_InitStruct);

  GPIO_InitStruct.GPIO_Pin = PIN_GREEN_LED;
  GPIO_Init(GPIO_GREEN_LED, &GPIO_InitStruct);
}

void GreenLEDon(void) {
  GPIO_WriteBit(GPIO_GREEN_LED, PIN_GREEN_LED, LED_ON);
}

void GreenLEDoff(void) {
  GPIO_WriteBit(GPIO_GREEN_LED, PIN_GREEN_LED, LED_OFF);
}

int GreenLEDstate(void) {
  return GPIO_ReadOutputDataBit(GPIO_GREEN_LED, PIN_GREEN_LED) == LED_ON;
}

void RedLEDon(void) {
  GPIO_WriteBit(GPIO_RED_LED, PIN_RED_LED, LED_ON);
}

void RedLEDoff(void) {
  GPIO_WriteBit(GPIO_RED_LED, PIN_RED_LED, LED_OFF);
}

int RedLEDstate(void) {
  return GPIO_ReadOutputDataBit(GPIO_RED_LED, PIN_RED_LED) == LED_ON;
}

static const unsigned delayTime = 3000000U;

void OK(int n) {
  int i;

  GreenLEDoff();
  for (i = 0; i < n; ++i) {
    Delay(delayTime);
    GreenLEDon();
    Delay(delayTime);
    GreenLEDoff();
  }
  Delay(9U * delayTime);
}

void Error(int n) {
  int i;

  RedLEDoff();
  for (i = 0; i < n; ++i) {
    Delay(delayTime);
    RedLEDon();
    Delay(delayTime);
    RedLEDoff();
  }
  Delay(9U * delayTime);
}
