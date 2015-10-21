#ifndef _SCSI_H
#define _SCSI_H 1

#include <stdint.h>

typedef enum {SCSI_OUT, SCSI_IN} scsi_direction_t;

void    SCSIreset(void);
uint8_t SCSIgetMaxLUN(void);
uint8_t SCSIcommand(uint8_t, uint8_t const *, uint8_t,
                    uint8_t **, uint32_t *, scsi_direction_t *);

#endif
