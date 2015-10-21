#include <board_init.h>
#include <delay.h>
#include <error.h>
#include <lcd_util.h>
#include <led.h>

int main(void) {
  static const unsigned delay_time = 2000000;
  int i;

  AllPinsDisable();
  LEDconfigure();
  ErrorResetable(ClockConfigure(32), 2);
  ErrorResetable(LCDconfigure(), 3);
  for (i = 0; ; ++i) {
    LCDwrite("HELLO");
    LCDwriteHex(1, i);
    RedLEDon();
    Delay(delay_time);
    GreenLEDon();
    Delay(delay_time);
    RedLEDoff();
    Delay(delay_time);
    GreenLEDoff();
    Delay(delay_time);
    LCDclear();
  }
}
