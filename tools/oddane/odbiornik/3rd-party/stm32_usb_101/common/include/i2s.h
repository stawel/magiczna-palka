#ifndef _I2S_H
#define _I2S_H 1

#include <stdint.h>

typedef enum {
  I2S_PLAY_ONCE = 0,
  I2S_PLAY_FOREVER = 1
} i2s_play_t;

typedef enum {
  I2S_NO_IRQ = 0,
  I2S_IRQ_TRANSFER_COMPLETE
} i2s_irq_source_t;

void I2Sconfigure(uint32_t);
int  I2Splay(int16_t const *, uint32_t, i2s_play_t play,
             i2s_irq_source_t, unsigned, unsigned,
             void (*)(int16_t **, uint32_t *));
void I2Spause(void);
void I2Sresume(void);

#endif
