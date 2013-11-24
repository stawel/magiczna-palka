#include <board_def.h>
#include <boot.h>

#define GPIO_BOOT1  xcat(GPIO, BOOT1_GPIO_N)
#define PIN_BOOT1   xcat(GPIO_Pin_, BOOT1_PIN_N)
#define RCC_BOOT1   xcat(RCC_APB2Periph_GPIO, BOOT1_GPIO_N)

void GetBootParams(int *sysclk, usb_speed_t *speed, usb_phy_t *phy) {
  if (sysclk) {
    GPIO_InitTypeDef GPIO_InitStruct;

    RCC_APB2PeriphClockCmd(RCC_BOOT1, ENABLE);
    GPIO_InitStruct.GPIO_Pin = PIN_BOOT1;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStruct.GPIO_Speed = 0;
    GPIO_Init(GPIO_BOOT1, &GPIO_InitStruct);
    if (GPIO_ReadInputDataBit(GPIO_BOOT1, PIN_BOOT1))
      *sysclk = 72;
    else
      *sysclk = 48;
    RCC_APB2PeriphClockCmd(RCC_BOOT1, DISABLE);
  }
  if (speed)
    *speed = FULL_SPEED;
  if (phy)
    *phy = USB_PHY_A;
}
