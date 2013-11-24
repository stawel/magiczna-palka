#ifndef _LIS302DL_H
#define _LIS302DL_H 1

#include <stdint.h>

/** LIS302DL configure structures **/

typedef struct {
  uint8_t Output_DataRate;
  uint8_t Power_Mode;
  uint8_t Full_Scale;
  uint8_t Self_Test;
  uint8_t Axes_Enable;
} LIS302DL_InitTypeDef;

#define LIS302DL_DATARATE_100            0x00
#define LIS302DL_DATARATE_400            0x80

#define LIS302DL_LOWPOWERMODE_POWERDOWN  0x00
#define LIS302DL_LOWPOWERMODE_ACTIVE     0x40

#define LIS302DL_FULLSCALE_2_3           0x00
#define LIS302DL_FULLSCALE_9_2           0x20

#define LIS302DL_SELFTEST_NORMAL         0x00
#define LIS302DL_SELFTEST_P              0x10
#define LIS302DL_SELFTEST_M              0x08

#define LIS302DL_X_ENABLE                0x01
#define LIS302DL_Y_ENABLE                0x02
#define LIS302DL_XY_ENABLE               0x03
#define LIS302DL_Z_ENABLE                0x04
#define LIS302DL_XYZ_ENABLE              0x07

typedef struct {
  uint8_t HighPassFilter_Data_Selection;
  uint8_t HighPassFilter_Interrupt;
  uint8_t HighPassFilter_CutOff_Frequency;
} LIS302DL_FilterConfigTypeDef;

#define LIS302DL_FILTEREDDATASELECTION_BYPASSED        0x00
#define LIS302DL_FILTEREDDATASELECTION_OUTPUTREGISTER  0x20

#define LIS302DL_HIGHPASSFILTERINTERRUPT_OFF           0x00
#define LIS302DL_HIGHPASSFILTERINTERRUPT_1             0x04
#define LIS302DL_HIGHPASSFILTERINTERRUPT_2             0x08
#define LIS302DL_HIGHPASSFILTERINTERRUPT_1_2           0x0C

#define LIS302DL_HIGHPASSFILTER_LEVEL_0                0x00
#define LIS302DL_HIGHPASSFILTER_LEVEL_1                0x01
#define LIS302DL_HIGHPASSFILTER_LEVEL_2                0x02
#define LIS302DL_HIGHPASSFILTER_LEVEL_3                0x03

typedef struct {
  uint8_t Latch_Request;
  uint8_t DoubleClick_Axes;
  uint8_t SingleClick_Axes;
} LIS302DL_InterruptConfigTypeDef;

#define LIS302DL_INTERRUPTREQUEST_NOTLATCHED       0x00
#define LIS302DL_INTERRUPTREQUEST_LATCHED          0x40

#define LIS302DL_DOUBLECLICKINTERRUPT_XYZ_DISABLE  0x00
#define LIS302DL_DOUBLECLICKINTERRUPT_X_ENABLE     0x02
#define LIS302DL_DOUBLECLICKINTERRUPT_Y_ENABLE     0x08
#define LIS302DL_DOUBLECLICKINTERRUPT_Z_ENABLE     0x20
#define LIS302DL_DOUBLECLICKINTERRUPT_XYZ_ENABLE   0x2A

#define LIS302DL_CLICKINTERRUPT_XYZ_DISABLE        0x00
#define LIS302DL_CLICKINTERRUPT_X_ENABLE           0x01
#define LIS302DL_CLICKINTERRUPT_Y_ENABLE           0x04
#define LIS302DL_CLICKINTERRUPT_Z_ENABLE           0x10
#define LIS302DL_CLICKINTERRUPT_XYZ_ENABLE         0x15

/* Not used, always 4-wire interface is selected.
#define LIS302DL_SERIALINTERFACE_4WIRE  0x00
#define LIS302DL_SERIALINTERFACE_3WIRE  0x80
*/

/** LIS302DL interface **/

void LIS302DL_Init(LIS302DL_InitTypeDef *);
void LIS302DL_FilterConfig(LIS302DL_FilterConfigTypeDef *);
void LIS302DL_InterruptConfig(LIS302DL_InterruptConfigTypeDef *);
void LIS302DL_DataRateCmd(uint8_t);
void LIS302DL_LowPowerCmd(uint8_t);
void LIS302DL_FullScaleCmd(uint8_t);
void LIS302DL_RebootCmd(void);

void LIS302DL_ReadACC(int32_t *);
void LIS302DL_ReadXYZ(int8_t *, uint32_t);

#endif /* _LIS302DL_H */
