#ifndef _CS43L22_H
#define _CS43L22_H 1

#include <stdint.h>

int      CS43L22_ResetConfigure(void);
int      CS43L22_Reset(int);
int      CS43L22_Init(unsigned, uint32_t, unsigned, unsigned,
                      void (*)(int16_t **, uint32_t *));
int      CS43L22_PauseResume(int);
int      CS43L22_Mute(int);
uint16_t CS43L22_GetResolution(void);
int16_t  CS43L22_GetVolumeMin(void);
int16_t  CS43L22_GetVolumeMax(void);
int      CS43L22_SetVolume(int16_t);

/* Possible parameter values for the CS43L22_Reset function */
#define CODEC_RESET_ON   0
#define CODEC_RESET_OFF  1

/* The first parameter of the CS43L22_Init function is
   a combination of the following values. */
#define INPUT_AIN1     0x01
#define INPUT_AIN2     0x02
#define INPUT_AIN3     0x04
#define INPUT_AIN4     0x08
#define INPUT_AIN_ALL  0x0F
#define INPUT_I2S      0x10
#define INPUT_ALL      0x1F

/* Possible parameter values for the CS43L22_PauseResume function */
#define AUDIO_PAUSE     0
#define AUDIO_RESUME    1

/* Possible parameter values for the CS43L22_Mute function */
#define AUDIO_MUTE_ON   1
#define AUDIO_MUTE_OFF  0

#endif
