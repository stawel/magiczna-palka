#ifndef _USBH_MSC_REQ_H
#define _USBH_MSC_REQ_H 1

#include <stdint.h>

int MSCgetMaxLun(int, uint8_t, uint8_t *);
int MSCresetBOT(int, uint8_t);
int MSCresetRecovery(int, uint8_t, uint8_t, uint8_t);

#endif
