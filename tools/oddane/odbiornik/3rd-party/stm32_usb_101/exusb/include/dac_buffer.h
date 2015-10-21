#ifndef _DAC_BUFFER_H
#define _DAC_BUFFER_H 1

#include <stdint.h>

/** Interface between dac_buffer.c and low level, device specific
    dac_12bits_*.c, dac_pwm_*.c implementations. **/

/* The parameter MDEV determines the maximal deviation of the sampling
   frequency. The parameter MPER defines the maximal period between
   receiving consecutive samples. */
#define MDEV  1
#define MPER  6
#define DAC_BUFF_SIZE  ((2 * MDEV + 1) * (1 << MPER) - 1)

/* Buffer is accessed directly because of performance reason. */
extern volatile uint32_t buffered, readPtr, writePtr;
extern volatile int16_t  buffer[DAC_BUFF_SIZE];

#endif
