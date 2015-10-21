#ifndef _JOYSTICK_H
#define _JOYSTICK_H 1

#define JOYSTICK_BUTTON_1  0x0001
#define JOYSTICK_BUTTON_2  0x0002
#define JOYSTICK_BUTTON_3  0x0004
#define JOYSTICK_UP        0x0010
#define JOYSTICK_DOWN      0x0020
#define JOYSTICK_RIGHT     0x0040
#define JOYSTICK_LEFT      0x0080

int JoystickConfigure(void);
int JoystickReset(void);
unsigned JoystickGetState(void);

#endif
