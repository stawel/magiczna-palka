#include <lcd_util.h>

/* Te funkcje nie są współużywalne. */

void LCDwrite(const char *s) {
  while (*s)
    LCDputchar(*s++);
}

void LCDwriteWrap(const char *s) {
  while (*s)
    LCDputcharWrap(*s++);
}

void LCDwriteLen(const char *s, unsigned n) {
  while (n-- > 0)
    LCDputchar(*s++);
}

void LCDwriteLenWrap(const char *s, unsigned n) {
  while (n-- > 0)
    LCDputcharWrap(*s++);
}

void LCDwriteHex(unsigned digits, unsigned hex) {
  unsigned d;

  while (digits > 0) {
    --digits;
    d = (hex >> (digits << 2)) & 0xf;
    if (d < 10)
      LCDputchar(d + '0');
    else
      LCDputchar(d + 'a' - 10);
  }
}

void LCDwriteHexWrap(unsigned digits, unsigned hex) {
  unsigned d;

  while (digits > 0) {
    --digits;
    d = (hex >> (digits << 2)) & 0xf;
    if (d < 10)
      LCDputcharWrap(d + '0');
    else
      LCDputcharWrap(d + 'a' - 10);
  }
}

static void (*callback)(void) = 0;

void LCDsetRefresh(void (*cb)(void)) {
  callback = cb;
}

void LCDrunRefresh(void) {
  register void (*f)(void);

  f = callback;
  if (f)
    f();
}
