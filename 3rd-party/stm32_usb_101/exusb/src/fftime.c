#include <ff.h>

DWORD get_fattime() { /* TODO: Implementation of RTC */
  return (2012 - 1980) << 25 | 11 << 21 | 13 << 16 | 22 << 11 | 18 << 5 | 4 >> 1;
}
