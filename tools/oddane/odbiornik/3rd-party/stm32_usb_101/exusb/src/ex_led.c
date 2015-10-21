#include <board_init.h>
#include <boot.h>
#include <delay.h>
#include <error.h>
#include <led.h>

int main(void) {
  static const unsigned delay_time = 2000000;
  int sysclk;

  GetBootParams(&sysclk, 0, 0);
  AllPinsDisable();
  LEDconfigure();
  ErrorResetable(ClockConfigure(sysclk), 2);

  for (;;) {
    RedLEDon();
    Delay(delay_time);
    GreenLEDon();
    Delay(delay_time);
    RedLEDoff();
    Delay(delay_time);
    GreenLEDoff();
    Delay(delay_time);
  }
}
