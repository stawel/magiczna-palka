#include <board_init.h>
#include <cs43l22.h>
#include <delay.h>
#include <error.h>
#include <irq.h>
#include <led.h>

/* Address in Flash, where raw audio data are stored, 16-bit per sample, stereo */
#define AUDIO_START_ADDRESS  0x8010000

/* Length in bytes of the raw audio data */
#define AUDIO_FILE_SIZE      0x00F0000

/* Audio data sampling frequency */
#define AUDIO_FREQ           48000

/* Size of the transfer (it must not exceed 0xFFFE) */
#define TRANSFER_SIZE        96

/* Function called back, when previous transfer is completed.
   It returns:
    - sample - a pointer to new samples to be played,
    - count  - the number of new samples. */
void TransferComplete(int16_t **samples, uint32_t *count) {
  static int16_t *CurrentSample = (int16_t *)AUDIO_START_ADDRESS;
  static uint32_t RemainingSamples = AUDIO_FILE_SIZE >> 1;

  if (RemainingSamples > TRANSFER_SIZE) {
    *samples = CurrentSample;
    *count = TRANSFER_SIZE;
    CurrentSample += TRANSFER_SIZE;
    RemainingSamples -= TRANSFER_SIZE;
  }
  else { /* RemainingSamples > 0 */
    *samples = CurrentSample;
    *count = RemainingSamples;
    CurrentSample = (int16_t *)AUDIO_START_ADDRESS;
    RemainingSamples = AUDIO_FILE_SIZE >> 1;
  }
}

int main(void) {
  AllPinsDisable();
  LEDconfigure();
  RedLEDon();
  IRQprotectionConfigure();
  ErrorResetable(ClockConfigure(120), 2);
  ErrorResetable(CS43L22_Init(INPUT_I2S, AUDIO_FREQ, HIGH_IRQ_PRIO, 0,
                              TransferComplete), 3);
  RedLEDoff();

//   Delay(10000000);
//   ErrorResetable(CS43L22_Mute(AUDIO_MUTE_ON), 4);
//   Delay(10000000);
//   ErrorResetable(CS43L22_Mute(AUDIO_MUTE_OFF), 5);

//   Delay(10000000);
//   ErrorResetable(CS43L22_SetVolume(12 << 8), 4);
//   Delay(10000000);
//   ErrorResetable(CS43L22_SetVolume(-9 << 8), 5);

//   Delay(10000000);
//   ErrorResetable(CS43L22_PauseResume(AUDIO_PAUSE), 4);
//   Delay(90000000);
//   ErrorResetable(CS43L22_PauseResume(AUDIO_RESUME), 5);

  for (;;);
}
