#include <board_init.h>
#include <boot.h>
#include <delay.h>
#include <error.h>
#include <irq.h>
#include <lcd_util.h>
#include <usbd_configure.h>
#include <usbd_power.h>

void ADC_Configuration(void);

int main(void) {
  int         sysclk;
  usb_speed_t speed;
  usb_phy_t   phy;

  GetBootParams(&sysclk, &speed, &phy);
  AllPinsDisable();
  LEDconfigure();
  RedLEDon();
  /* USB should be configured just after power on. In the other case
     the pull-up resistor must be kept off until configuration is not
     finished. */
  ErrorResetable(USBDpreConfigure(speed, phy), 1);
  /* Wait about 0.5 to 2 s for JTAG before you configure the clock.
     Simulation of a long-term operation, which makes necessary
     to divide USB configuration into two stages. */
  Delay(2000000);
  IRQprotectionConfigure();
  ErrorResetable(ClockConfigure(sysclk), 2);
  ErrorResetable(PWRconfigure(HIGH_IRQ_PRIO, 0, sysclk), 4);
  ErrorResetable(USBDconfigure(MIDDLE_IRQ_PRIO, 0, sysclk), 5);
  RedLEDoff();
  /* Configuration process is finished successfully. */
  for (;;);
}
