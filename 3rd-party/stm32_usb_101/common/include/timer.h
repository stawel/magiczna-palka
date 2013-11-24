#ifndef _TIMER_H
#define _TIMER_H 1

void TimerConfigure(unsigned, unsigned);
void TimerStart(int, void (*)(void), unsigned);
void TimerStop(int);
void ActiveWait(int, unsigned);

void FineTimerConfigure(unsigned, unsigned);
void FineTimerStart(int, void (*)(void), unsigned);
void FineTimerStop(int);

#endif
