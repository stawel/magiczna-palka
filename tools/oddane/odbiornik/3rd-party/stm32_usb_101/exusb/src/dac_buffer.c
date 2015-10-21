#include <dac.h>
#include <dac_buffer.h>
#include <irq.h>

/** Internals **/

#define MIN_VOL_DB  -42
#define MAX_VOL_DB  42
#define VOLUME_MIN  (MIN_VOL_DB * 256)
#define VOLUME_MAX  (MAX_VOL_DB * 256)
#define VOLUME_RES  256

static const int32_t gainTbl[MAX_VOL_DB - MIN_VOL_DB + 1] = {
     4,     5,     5,     6,     6,     7,     8,     9,    10,    11,
    13,    14,    16,    18,    20,    23,    26,    29,    32,    36,
    41,    46,    51,    57,    64,    72,    81,    91,   102,   115,
   129,   144,   162,   182,   204,   229,   257,   288,   323,   362,
   407,   456,   512,   574,   645,   723,   811,   910,  1022,  1146,
  1286,  1443,  1619,  1817,  2038,  2287,  2566,  2879,  3231,  3625,
  4067,  4563,  5120,  5745,  6446,  7232,  8115,  9105, 10216, 11462,
 12861, 14430, 16191, 18166, 20383, 22870, 25661, 28792, 32305, 36247,
 40670, 45632, 51200, 57447, 64457
};

static int32_t  gain;
static int16_t  volume;     /* in 1/256 dB */
static uint16_t resolution; /* in 1/256 dB */
static uint8_t  mute;

volatile uint32_t buffered, readPtr, writePtr;
volatile int16_t  buffer[DAC_BUFF_SIZE];

static int16_t Gain(int16_t sample) {
  if (mute == 1)
    return 0;
  else {
    int32_t s;

    s = sample >= 0 ? 256 : -256;
    s = (sample * gain + s) >> 9;
    if (s > 32767)
      return 32767;
    else if (s < -32768)
      return -32768;
    else
      return (int16_t)s;
  }
}

/** Public interface **/

void DACreset() {
  irq_level_t level;

  volume = 0;
  gain = gainTbl[(volume >> 8) - MIN_VOL_DB];
  resolution = VOLUME_RES;
  mute = 0;
  level = IRQprotect(HIGH_IRQ_PRIO);
  buffered = readPtr = writePtr = 0;
  IRQunprotect(level);
}

uint8_t DACgetMute() {
  return mute;
}

void DACsetMute(uint8_t new_mute) {
  mute = new_mute;
}

int16_t DACgetVolume() {
  return volume;
}

int16_t DACgetVolumeMin() {
  return VOLUME_MIN;
}

int16_t DACgetVolumeMax() {
  return VOLUME_MAX;
}

void DACsetVolume(int16_t new_voulume) {
  int g;

  volume = new_voulume;
  g = (volume >> 8) - MIN_VOL_DB;
  if (g < 0)
    g = 0;
  else if (g > MAX_VOL_DB - MIN_VOL_DB)
    g = MAX_VOL_DB - MIN_VOL_DB;
  gain = gainTbl[g];
}

uint16_t DACgetResolution() {
  return resolution;
}

void DACsetResolution(uint16_t new_resolution) {
  resolution = new_resolution;
}

void DACputSamples(int16_t const *samples, uint32_t count) {
  irq_level_t level;
  uint32_t i;
  int16_t  s;

  for (i = 0; i < count; ++i) {
    s = Gain(samples[i]);
    level = IRQprotect(HIGH_IRQ_PRIO);
    if (buffered < DAC_BUFF_SIZE) {
      buffer[writePtr] = s;
      ++buffered;
      if (writePtr < DAC_BUFF_SIZE - 1)
        ++writePtr;
      else
        writePtr = 0;
    }
    IRQunprotect(level);
  }
}
