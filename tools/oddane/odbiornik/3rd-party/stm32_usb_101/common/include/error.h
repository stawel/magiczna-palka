#ifndef _ERROR_H
#define _ERROR_H 1

#include <led.h>
#include <stm32.h>

/* Obsługa błędów, których przyczyna wydaje się być nieusuwalna. */
static inline void ErrorPermanent(int expr, int err) {
  if (expr < 0)
    for (;;)
      Error(err);
}

/* Obsługa błędów, których przyczyna może ustąpić po wyzerowaniu. */
static inline void ErrorResetable(int expr, int err) {
  if (expr < 0) {
    int i;
    for (i = 0; i < 3; ++i)
      Error(err);
    NVIC_SystemReset();
  }
}

#endif
