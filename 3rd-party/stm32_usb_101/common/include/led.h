#ifndef _LED_H
#define _LED_H 1

void LEDconfigure(void);
void GreenLEDon(void);
void GreenLEDoff(void);
int GreenLEDstate(void);
void RedLEDon(void);
void RedLEDoff(void);
int RedLEDstate(void);
void OK(int);
void Error(int);

#endif
