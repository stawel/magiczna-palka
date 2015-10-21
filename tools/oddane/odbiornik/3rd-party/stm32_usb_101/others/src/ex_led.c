#include <board_init.h>
#include <delay.h>
#include <error.h>
#include <irq.h>
#include <led.h>

int main(void) {
  static const unsigned delay_time = 2000000;
  irq_level_t level;

  AllPinsDisable();
  LEDconfigure();
  IRQprotectionConfigure();
  ErrorResetable(ClockConfigure(48), 2);
  for (;;) {
    level = IRQprotect(HIGH_IRQ_PRIO);
    RedLEDon();
    Delay(delay_time);
    GreenLEDon();
    Delay(delay_time);
    RedLEDoff();
    Delay(delay_time);
    GreenLEDoff();
    Delay(delay_time);
    IRQunprotect(level);
  }
}
