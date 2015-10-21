#include <board_def.h>
#include <boot.h>

#define GPIO_BOOT1  xcat(GPIO, BOOT1_GPIO_N)
#define PIN_BOOT1   xcat(GPIO_Pin_, BOOT1_PIN_N)
#define RCC_BOOT1   xcat(RCC_AHB1Periph_GPIO, BOOT1_GPIO_N)
#define GPIO_BOOT2  xcat(GPIO, BOOT2_GPIO_N)
#define PIN_BOOT2   xcat(GPIO_Pin_, BOOT2_PIN_N)
#define RCC_BOOT2   xcat(RCC_AHB1Periph_GPIO, BOOT2_GPIO_N)

void GetBootParams(int *sysclk, usb_speed_t *speed, usb_phy_t *phy) {
  GPIO_InitTypeDef GPIO_InitStruct;
  int boot1, boot2;

  RCC_AHB1PeriphClockCmd(RCC_BOOT1 | RCC_BOOT2, ENABLE);
  GPIO_InitStruct.GPIO_Pin = PIN_BOOT1;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIO_BOOT1, &GPIO_InitStruct);
  GPIO_InitStruct.GPIO_Pin = PIN_BOOT2;
  GPIO_Init(GPIO_BOOT2, &GPIO_InitStruct);
  boot1 = GPIO_ReadInputDataBit(GPIO_BOOT1, PIN_BOOT1);
  boot2 = GPIO_ReadInputDataBit(GPIO_BOOT2, PIN_BOOT2);
  RCC_AHB2PeriphClockCmd(RCC_BOOT1 | RCC_BOOT2, DISABLE);

  if (sysclk)
    *sysclk = boot2 || boot1 ? 120 : 48;
  if (speed)
    *speed = boot2 && boot1 ? HIGH_SPEED : FULL_SPEED;
  if (phy)
    *phy = boot2 ? USB_PHY_ULPI : USB_PHY_A;
}
