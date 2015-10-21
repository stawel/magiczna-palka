#include <board_init.h>
#include <error.h>
#include <led.h>
#include <math.h>

/* Floating point hardware must be enabled before calling any
   function which uses floating point instructions. This could
   be done in the startup before main is called. */
static void EnableFPU(void) {
  #if __FPU_USED == 1
  SCB->CPACR |= 0xf << 20;
  __DSB();
  __ISB();
  #endif
}

/* A simple test if FPU is working. This function must not be inlined,
   because code optimization could cause in emitting floating point
   instructions before EnableFPU is executed. */
__attribute__ ((noinline)) static void UseFPU(void) {
  volatile float x, y; /* Do not optimize these variables. */

  for (x = 1.5F; x < 50.0F; x = 2.2F * x + 0.8F) {
    y = sqrtf(x);
    OK((int)y);
  }
}

int main(void) {
  int sysclk;

  #if defined STM32F0XX
  sysclk = 48;
  #elif defined STM32F4XX
  sysclk = 168;
  #endif

  AllPinsDisable();
  LEDconfigure();
  ErrorResetable(ClockConfigure(sysclk), 2);

  EnableFPU();
  for (;;)
    UseFPU();
}

void FPU_IRQHandler(void) {
  Error(3);
}

void UsageFault_Handler(void) {
  Error(4);
}

void HardFault_Handler(void) {
  Error(5);
}
