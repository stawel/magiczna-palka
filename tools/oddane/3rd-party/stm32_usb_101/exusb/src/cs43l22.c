#include <cs43l22.h>
#include <i2c.h>
#include <i2s.h>

/** I2C chips adresses and IDs **/

#define CS43L22_ADDRESS  0x94
#define CS43L22_ID       0xE0
#define CS43L22_ID_MASK  0xF8

/** Volume range **/

/* The following values are in dB. */
#define MIN_VOL_DB  -102
#define MAX_VOL_DB  12

/* The following values are in 1/256 dB. */
#define VOLUME_RES  128
#define VOLUME_MIN  (MIN_VOL_DB << 8)
#define VOLUME_MAX  (MAX_VOL_DB << 8)

/** Internals **/

#define SILENCE_LEN  16
static const int16_t Silence[SILENCE_LEN];

/** Public interface **/

/* Initialize the audio chip in stereo mode. Set input, sampling
   frequency, interrupt preemption priority and subpriority, transfer
   complete callback. */
int CS43L22_Init(unsigned input, uint32_t frequency,
                 unsigned prio, unsigned subprio,
                 void (* callback)(int16_t **, uint32_t *)) {
  uint8_t value;

  I2Cconfigure();

  if (CS43L22_ResetConfigure() < 0)
    return -1;

  I2Sconfigure(frequency);

  if (CS43L22_Reset(CODEC_RESET_OFF) < 0)
    return -1;

  /* Check if the device is operational. */
  if (I2CreadDeviceRegister(CS43L22_ADDRESS, 0x01, &value) < 0)
    return -1;
  if ((value & CS43L22_ID_MASK) != CS43L22_ID)
    return -1;

  /* Speaker always off, headphones always on */
  if (I2CwriteDeviceRegister(CS43L22_ADDRESS, 0x04, 0xAF) < 0)
    return -1;

  /* Set the slave mode and the audio standard. */
  if (I2CwriteDeviceRegister(CS43L22_ADDRESS, 0x05, 0x81) < 0)
    return -1;
  if (I2CwriteDeviceRegister(CS43L22_ADDRESS, 0x06, 0x04) < 0)
    return -1;

  value = input & INPUT_AIN_ALL;

  /* Enable analog inputs. */
  if (I2CwriteDeviceRegister(CS43L22_ADDRESS, 0x08, value) < 0)
    return -1;
  if (I2CwriteDeviceRegister(CS43L22_ADDRESS, 0x09, value) < 0)
    return -1;

  if (value) {
    /* Enable analog inputs and set -9 dB gain. */
    if (I2CwriteDeviceRegister(CS43L22_ADDRESS, 0x0E, 0xC0) < 0)
      return -1;
    if (I2CwriteDeviceRegister(CS43L22_ADDRESS, 0x14, 0xEE) < 0)
      return -1;
    if (I2CwriteDeviceRegister(CS43L22_ADDRESS, 0x15, 0xEE) < 0)
      return -1;
  }

  /* Put the audio chip in the power up mode. */
  if (I2CwriteDeviceRegister(CS43L22_ADDRESS, 0x02, 0x9E) < 0)
    return -1;

  if (input & INPUT_I2S) {
    /* Play initial silence and then call the callback function. */
    return I2Splay(Silence, SILENCE_LEN, I2S_PLAY_ONCE,
                   I2S_IRQ_TRANSFER_COMPLETE, prio, subprio, callback);
  }
  else {
    /* Play silence on the I2S input. */
    return I2Splay(Silence, SILENCE_LEN, I2S_PLAY_FOREVER,
                   I2S_NO_IRQ, 0, 0, 0);
  }
}

int CS43L22_PauseResume(int cmd) {
  if (cmd == AUDIO_PAUSE) {
    if (CS43L22_Mute(AUDIO_MUTE_ON) < 0)
      return -1;

    I2Spause();

    /* Put the audio chip in the power down mode. */
    return I2CwriteDeviceRegister(CS43L22_ADDRESS, 0x02, 0x01);
  }
  else if (cmd == AUDIO_RESUME) {
    /* Exit the power down mode. */
    if (I2CwriteDeviceRegister(CS43L22_ADDRESS, 0x02, 0x9E) < 0)
      return -1;

    I2Sresume();

    return CS43L22_Mute(AUDIO_MUTE_OFF);
  }
  else {
    return -1;
  }
}

/* If cmd != 0 then switch sound off. */
int CS43L22_Mute(int cmd) {
  if (cmd == AUDIO_MUTE_ON) {
    /* Speaker always off, headphones always off */
    return I2CwriteDeviceRegister(CS43L22_ADDRESS, 0x04, 0xFF);
  }
  else {
    /* Speaker always off, headphones always on */
    return I2CwriteDeviceRegister(CS43L22_ADDRESS, 0x04, 0xAF);
  }
}

/* Return value is in 1/256 dB. */
uint16_t CS43L22_GetVolumeResolution() {
  return VOLUME_RES;
}

/* Return value is in 1/256 dB. */
int16_t CS43L22_GetVolumeMin() {
  return VOLUME_MIN;
}

/* Return value is in 1/256 dB. */
int16_t CS43L22_GetVolumeMax(void) {
  return VOLUME_MAX;
}

/* Set sound volume. The value is in 1/256 dB. */
int CS43L22_SetVolume(int16_t volume) {
  uint8_t v;

  if (volume > VOLUME_MAX)
    v = (VOLUME_MAX >> 7) & 0xFF;
  else if (volume >= VOLUME_MIN)
    v = (volume >> 7) & 0xFF;
  else
    v = (VOLUME_MIN >> 7) & 0xFF;

  if (I2CwriteDeviceRegister(CS43L22_ADDRESS, 0x20, v) < 0)
    return -1;
  if (I2CwriteDeviceRegister(CS43L22_ADDRESS, 0x21, v) < 0)
    return -1;

  return 0;
}
