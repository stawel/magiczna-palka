#ifndef _LCD_UTIL_H
#define _LCD_UTIL_H 1

#include <lcd.h>

void LCDwrite(const char *);
void LCDwriteWrap(const char *);
void LCDwriteLen(const char *, unsigned);
void LCDwriteLenWrap(const char *, unsigned);
void LCDwriteHex(unsigned, unsigned);
void LCDwriteHexWrap(unsigned, unsigned);
void LCDsetRefresh(void (*)(void));
void LCDrunRefresh(void);

#endif
