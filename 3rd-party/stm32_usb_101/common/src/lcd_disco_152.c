#include <active_check.h>
#include <board_def.h>
#include <lcd.h>

/** Character mapping **/

/*
   ---A---
  |\  |  /|
  F H J K B  _
  |  \|/  | |_| COL
   -G- -M-   _
  |  /|\  | |_| COL
  E Q P N C
  |/  |  \|  _
   ---D---  |_| DP

An LCD character coding is based on the following matrix:
      E, D, P,   N,
      M, C, COL, DP,
      B, A, K,   J,
      G, F, Q,   H.

The character 'A' for example is:
LSB   1, 0, 0, 0,
      1, 1, 0, 0,
      1, 1, 0, 0,
MSB   1, 1, 0, 0.

'A' = 0xFE00
*/

#define C_star          0xA0DDU
#define C_minus         0xA000U
#define C_slash         0x00C0U
#define C_backslash     0x0009U
#define C_vertical_bar  0x0014U
#define C_dot           0x0002U
#define C_colon         0x0020U

static const uint16_t DigitMap[10] = {
  /* 0       1       2       3       4       5       6       7       8       9 */
  0x5F00, 0x4200, 0xF500, 0x6700, 0xEA00, 0xAF00, 0xBF00, 0x4600, 0xFF00, 0xEF00
};

static const uint16_t CapitalLetterMap[26] = {
  /* A       B       C       D       E       F       G       H       I */
  0xFE00, 0x6714, 0x1D00, 0x4714, 0x9D00, 0x9C00, 0x3F00, 0xFA00, 0x0014,
  /* J       K       L       M       N       O       P       Q       R */
  0x5300, 0x9841, 0x1900, 0x5a48, 0x5a09, 0x5f00, 0xFC00, 0x5F01, 0xFC01,
  /* S       T       U       V       W       X       Y       Z */
  0xAF00, 0x0414, 0x5b00, 0x18c0, 0x5a81, 0x00c9, 0x0058, 0x05c0
};

/* Convert an ASCII character to a LCD segment coding. */
static uint32_t LCDchar2segment(uint8_t c) {
  if (c >= '0' && c <= '9')
    return DigitMap[c - '0'];
  else if (c >= 'A' && c <= 'Z')
    return CapitalLetterMap[c - 'A'];
  else if (c >= 'a' && c <= 'z')
    return CapitalLetterMap[c - 'a'];
  else {
    switch (c) {
      case '*':
        return C_star;
      case '-':
        return C_minus;
      case '/':
        return C_slash;
      case '\\':
        return C_backslash;
      case '|':
        return C_vertical_bar;
      default:
        return 0;
    }
  }
}

/** Initialization **/

/* Configure the 32768 Hz RTC quarz oscilator.
   TODO: This should be moved to an RTC module. */
static int RTCclockConfigure(void) {
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
  PWR_RTCAccessCmd(ENABLE);

  /* Switch LSE on and wait until it is ready. */
  RCC_LSEConfig(RCC_LSE_ON);
  active_check(RCC_GetFlagStatus(RCC_FLAG_LSERDY), 1000000);

  /* Select RTC clock and enable it. */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
  RCC_RTCCLKCmd(ENABLE);

  return 0;
}

static void LCDglassConfigureGPIO(void) {
  GPIO_InitTypeDef GPIO_InitStruct;

  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA |
                        RCC_AHBPeriph_GPIOB |
                        RCC_AHBPeriph_GPIOC,
                        ENABLE);

  GPIO_StructInit(&GPIO_InitStruct);
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;

  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1  | GPIO_Pin_2 | GPIO_Pin_3  |
                             GPIO_Pin_8  | GPIO_Pin_9 | GPIO_Pin_10 |
                             GPIO_Pin_15;
  GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_PinAFConfig(GPIOA, GPIO_PinSource1,  GPIO_AF_LCD);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource2,  GPIO_AF_LCD);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource3,  GPIO_AF_LCD);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource8,  GPIO_AF_LCD);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource9,  GPIO_AF_LCD);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_LCD);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource15, GPIO_AF_LCD);

  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3  | GPIO_Pin_4  | GPIO_Pin_5  |
                             GPIO_Pin_8  | GPIO_Pin_9  | GPIO_Pin_10 |
                             GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 |
                             GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_Init(GPIOB, &GPIO_InitStruct);

  GPIO_PinAFConfig(GPIOB, GPIO_PinSource3,  GPIO_AF_LCD);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource4,  GPIO_AF_LCD);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource5,  GPIO_AF_LCD);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource8,  GPIO_AF_LCD);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource9,  GPIO_AF_LCD);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_LCD);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_LCD);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource12, GPIO_AF_LCD);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_LCD);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource14, GPIO_AF_LCD);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_LCD);

  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0  | GPIO_Pin_1  | GPIO_Pin_2  |
                             GPIO_Pin_3  | GPIO_Pin_6  | GPIO_Pin_7  |
                             GPIO_Pin_8  | GPIO_Pin_9  | GPIO_Pin_10 |
                             GPIO_Pin_11;
  GPIO_Init(GPIOC, &GPIO_InitStruct);

  GPIO_PinAFConfig(GPIOC, GPIO_PinSource0,  GPIO_AF_LCD);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource1,  GPIO_AF_LCD);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource2,  GPIO_AF_LCD);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource3,  GPIO_AF_LCD);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource6,  GPIO_AF_LCD);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource7,  GPIO_AF_LCD);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource8,  GPIO_AF_LCD);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource9,  GPIO_AF_LCD);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_LCD);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_LCD);

  /* Now GPIOs clock could be disabled for power saving,
     but other modules may also use these GPIOs. */
}

static int LCDglassConfigurePeripheral(void) {
  LCD_InitTypeDef LCD_InitStruct;

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_LCD, ENABLE);

  LCD_InitStruct.LCD_Prescaler = LCD_Prescaler_1;
  LCD_InitStruct.LCD_Divider = LCD_Divider_31;
  LCD_InitStruct.LCD_Duty = LCD_Duty_1_4;
  LCD_InitStruct.LCD_Bias = LCD_Bias_1_3;
  LCD_InitStruct.LCD_VoltageSource = LCD_VoltageSource_Internal;
  LCD_Init(&LCD_InitStruct);

  LCD_MuxSegmentCmd(ENABLE);

  /* Set contrast to the mean value. */
  LCD_ContrastConfig(LCD_Contrast_Level_4);

  LCD_DeadTimeConfig(LCD_DeadTime_0);
  LCD_PulseOnDurationConfig(LCD_PulseOnDuration_4);

  /* Wait until the LCD FCR register is synchronized. */
  LCD_WaitForSynchro();

  /* Enable LCD peripheral then wait until it is enabled and
     until the LCD booster is ready. */
  LCD_Cmd(ENABLE);
  active_check(LCD_GetFlagStatus(LCD_FLAG_ENS), 1000000);
  active_check(LCD_GetFlagStatus(LCD_FLAG_RDY), 1000000);

  LCD_BlinkConfig(LCD_BlinkMode_Off, LCD_BlinkFrequency_Div32);

  return 0;
}

/** LCD internal API **/

static void LCDRAM(uint32_t reg, uint32_t mask, uint32_t value) {
  uint32_t x;

  x = LCD->RAM[reg];
  x &= mask;
  x |= value;
  LCD->RAM[reg] = x;
}

/* Write bar.
    bar - Four lowest significant bits corespond to LCD bars. */
static int LCDglassWriteBar(uint32_t bar) {
  /* Wait for LCD ready. */
  active_check(LCD_GetFlagStatus(LCD_FLAG_UDR) == RESET, 1000000);

  LCDRAM(LCD_RAMRegister_4, 0xffff5fff, (bar << 14 & 0x8000) | (bar << 10 & 0x2000));
  LCDRAM(LCD_RAMRegister_6, 0xffff5fff, (bar << 15 & 0x8000) | (bar << 11 & 0x2000));

  LCD_UpdateDisplayRequest();
  return 0;
}

/* Write a character in the LCD frame buffer.
    c        - the character to display
    cs       - coded segments
    position - position of the caracter in the LCD (0 to 5) */
static int LCDglassWriteChar(char c, uint32_t cs, int position) {
  cs |= LCDchar2segment(c);

  /* Wait for LCD ready. */
  active_check(LCD_GetFlagStatus(LCD_FLAG_UDR) == RESET, 1000000);

  switch (position) {
    case 0:
      LCDRAM(LCD_RAMRegister_0, 0xcffffffc, (cs << 14 & 0x30000000) | (cs >> 12 & 0x3));
      LCDRAM(LCD_RAMRegister_2, 0xcffffffc, (cs << 18 & 0x30000000) | (cs >>  8 & 0x3));
      LCDRAM(LCD_RAMRegister_4, 0xcffffffc, (cs << 22 & 0x30000000) | (cs >>  4 & 0x3));
      LCDRAM(LCD_RAMRegister_6, 0xcffffffc, (cs << 26 & 0x30000000) | (cs       & 0x3));
      break;
    case 1:
      LCDRAM(LCD_RAMRegister_0, 0xf3ffff7b, (cs << 12 & 0xc000000) | (cs >> 6 & 0x80) | (cs >> 10 & 0x4));
      LCDRAM(LCD_RAMRegister_2, 0xf3ffff7b, (cs << 16 & 0xc000000) | (cs >> 2 & 0x80) | (cs >>  6 & 0x4));
      LCDRAM(LCD_RAMRegister_4, 0xf3ffff7b, (cs << 20 & 0xc000000) | (cs << 2 & 0x80) | (cs >>  2 & 0x4));
      LCDRAM(LCD_RAMRegister_6, 0xf3ffff7b, (cs << 24 & 0xc000000) | (cs << 6 & 0x80) | (cs <<  2 & 0x4));
      break;
    case 2:
      LCDRAM(LCD_RAMRegister_0, 0xfcfffcff, (cs << 10 & 0x3000000) | (cs >> 4 & 0x300));
      LCDRAM(LCD_RAMRegister_2, 0xfcfffcff, (cs << 14 & 0x3000000) | (cs      & 0x300));
      LCDRAM(LCD_RAMRegister_4, 0xfcfffcff, (cs << 18 & 0x3000000) | (cs << 4 & 0x300));
      LCDRAM(LCD_RAMRegister_6, 0xfcfffcff, (cs << 22 & 0x3000000) | (cs << 8 & 0x300));
      break;
    case 3:
      LCDRAM(LCD_RAMRegister_0, 0xffcff3ff, (cs <<  6 & 0x300000) | (cs >>  2 & 0xc00));
      LCDRAM(LCD_RAMRegister_2, 0xffcff3ff, (cs << 10 & 0x300000) | (cs <<  2 & 0xc00));
      LCDRAM(LCD_RAMRegister_4, 0xffcff3ff, (cs << 14 & 0x300000) | (cs <<  6 & 0xc00));
      LCDRAM(LCD_RAMRegister_6, 0xffcff3ff, (cs << 18 & 0x300000) | (cs << 10 & 0xc00));
      break;
    case 4:
      LCDRAM(LCD_RAMRegister_0, 0xfff3cfff, (cs <<  4 & 0xc0000) | (cs       & 0x3000));
      LCDRAM(LCD_RAMRegister_2, 0xfff3cfff, (cs <<  8 & 0xc0000) | (cs <<  4 & 0x3000));
      LCDRAM(LCD_RAMRegister_4, 0xfff3efff, (cs << 12 & 0xc0000) | (cs <<  8 & 0x1000));
      LCDRAM(LCD_RAMRegister_6, 0xfff3efff, (cs << 16 & 0xc0000) | (cs << 12 & 0x1000));
      break;
    case 5:
      LCDRAM(LCD_RAMRegister_0, 0xfffc3fff, (cs <<  3 & 0x20000) | (cs <<  1 & 0x10000) | (cs <<  2 & 0xc000));
      LCDRAM(LCD_RAMRegister_2, 0xfffc3fff, (cs <<  7 & 0x20000) | (cs <<  5 & 0x10000) | (cs <<  6 & 0xc000));
      LCDRAM(LCD_RAMRegister_4, 0xfffcbfff, (cs << 11 & 0x20000) | (cs <<  9 & 0x10000) | (cs << 10 & 0x4000));
      LCDRAM(LCD_RAMRegister_6, 0xfffcbfff, (cs << 15 & 0x20000) | (cs << 13 & 0x10000) | (cs << 14 & 0x4000));
      break;
  }

  LCD_UpdateDisplayRequest();
  return 0;
}

/** LCD API **/

/* Screen organization */
#define SCREEN_HEIGHT  1
#define SCREEN_WIDTH   6

/* Current cursor position, x is the line number from top, and y is
   the column number from left. */
static int x, y;

int LCDconfigure() {
  if (RTCclockConfigure() < 0)
    return -1;
  LCDglassConfigureGPIO();
  if (LCDglassConfigurePeripheral() < 0)
    return -1;
  LCDclear();
  return 0;
}

void LCDclear() {
  unsigned i;

  x = y = 0;

  /* Wait for LCD ready. */
  while (LCD_GetFlagStatus(LCD_FLAG_UDR) != RESET);

  /* Clear the whole LCD RAM. */
  for (i = LCD_RAMRegister_0; i <= LCD_RAMRegister_15; ++i)
    LCD->RAM[i] = 0;

  /* Update the LCD display. */
  LCD_UpdateDisplayRequest();
}

void LCDgoto(int textLine, int charPos) {
  x = textLine;
  y = charPos;
}

void LCDputchar(char c) {
  static char frame_buffer[SCREEN_HEIGHT][SCREEN_WIDTH];

  if (c == '\n') {
    LCDgoto(x + 1, 0); /* new line */
  }
  else if (c == '\r') {
    LCDgoto(x, 0); /* carriage return */
  }
  else if (c == '\t') {
    LCDgoto(x, (y + 8) & ~7); /* tabulator */
  }
  else if (c == '.') {
    /* Only 4 first LCD positions displays dot after character. */
    if (x >= 0 && x < SCREEN_HEIGHT &&
        y >= 1 && y < SCREEN_WIDTH - 1) {
      LCDglassWriteChar(frame_buffer[x][y - 1], C_dot, y - 1);
    }
  }
  else if (c == ':') {
    /* Only 4 first LCD positions displays colon after character. */
    if (x >= 0 && x < SCREEN_HEIGHT &&
        y >= 1 && y < SCREEN_WIDTH - 1) {
      LCDglassWriteChar(frame_buffer[x][y - 1], C_colon, y - 1);
    }
  }
  else if (x >= 0 && x < SCREEN_HEIGHT &&
           y >= 0 && y < SCREEN_WIDTH) {
    frame_buffer[x][y] = c;
    LCDglassWriteChar(c, 0, y);
    LCDgoto(x, y + 1);
  }
  else if (x == 0 && y == 12) { /* Hard trick */
    if (c == '|')
      LCDglassWriteBar(1);
    else if (c == '/')
      LCDglassWriteBar(2);
    else if (c == '-')
      LCDglassWriteBar(4);
    else if (c == '\\')
      LCDglassWriteBar(8);
    else
      LCDglassWriteBar(0);
    LCDgoto(x, y + 1);
  }
  else { /* Hard trick continued */
    LCDgoto(x, y + 1);
  }
}

void LCDputcharWrap(char c) {
  if (y >= SCREEN_WIDTH)
    LCDputchar('\n'); /* There is no room for the next character, wrap. */
  LCDputchar(c);
}
