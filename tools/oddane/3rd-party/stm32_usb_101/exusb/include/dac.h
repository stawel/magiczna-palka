#ifndef _DAC_H
#define _DAC_H 1

#include <stdint.h>

int      DACconfigure(unsigned);
void     DACreset(void);
uint8_t  DACgetMute(void);
void     DACsetMute(uint8_t new_mute);
int16_t  DACgetVolume(void);
int16_t  DACgetVolumeMin(void);
int16_t  DACgetVolumeMax(void);
void     DACsetVolume(int16_t);
uint16_t DACgetResolution(void);
void     DACsetResolution(uint16_t);
void     DACputSamples(int16_t const *, uint32_t);

#endif
