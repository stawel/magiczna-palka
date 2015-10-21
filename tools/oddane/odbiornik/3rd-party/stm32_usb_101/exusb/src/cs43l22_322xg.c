#include <cs43l22.h>
#include <ioe_stm322xg.h>

/* The audio chip reset is connected to pin 2 of the second expander. */
int CS43L22_ResetConfigure() {
  if (IOE2configure() < 0)
    return -1;

  /* Initially the audio chip should be in the reset state. */
  return IOE2audioReset(0);
}

/* Control the state of the audio chip reset. */
int CS43L22_Reset(int state) {
  return IOE2audioReset(state);
}
