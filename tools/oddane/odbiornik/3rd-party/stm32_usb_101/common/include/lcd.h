#ifndef _LCD_H
#define _LCD_H 1

int  LCDconfigure(void);
void LCDclear(void);
void LCDgoto(int, int);
void LCDputchar(char);
void LCDputcharWrap(char);

#endif
