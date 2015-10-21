#ifndef _IOE_STM322XG_H
#define _IOE_STM322XG_H 1

/* Expander pins */
#define IO_Pin_0    0x01
#define IO_Pin_1    0x02
#define IO_Pin_2    0x04
#define IO_Pin_3    0x08
#define IO_Pin_4    0x10
#define IO_Pin_5    0x20
#define IO_Pin_6    0x40
#define IO_Pin_7    0x80

/* Joystick pins */
#define JOY_IO_SEL    IO_Pin_7
#define JOY_IO_DOWN   IO_Pin_6
#define JOY_IO_LEFT   IO_Pin_5
#define JOY_IO_RIGHT  IO_Pin_4
#define JOY_IO_UP     IO_Pin_3
#define JOY_IO_PINS   (IO_Pin_3 | IO_Pin_4 | IO_Pin_5 | IO_Pin_6 | IO_Pin_7)

int      IOE2configure(void);
int      IOE2audioReset(int);
unsigned IOE2joystickGetState(void);

#endif
