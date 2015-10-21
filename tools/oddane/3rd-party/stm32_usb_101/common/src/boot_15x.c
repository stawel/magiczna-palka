#include <board_def.h>
#include <boot.h>

void GetBootParams(int *sysclk, usb_speed_t *speed, usb_phy_t *phy) {
  if (sysclk)
    *sysclk = 32;
  if (speed)
    *speed = FULL_SPEED;
  if (phy)
    *phy = USB_PHY_A;
}
