#include <delay.h>
#include <fonts.h>
#include <lcd.h>

#include <stm32f2xx_fsmc.h>
#include <stm32f2xx_gpio.h>
#include <stm32f2xx_rcc.h>

/** The simple LCD driver (only text mode) for AM-240320L8TNQW00H
    (LCD_ILI9320) and AM240320D5TOQW01H (LCD_ILI9325) Liquid Crystal
    Display Modules of STM322xG_EVAL board **/

/* Screen size in pixels, left top corner has coordinates (0, 0). */
#define LCD_PIXEL_WIDTH   320
#define LCD_PIXEL_HEIGHT  240

#define LCD_COLOR_WHITE    0xFFFF
#define LCD_COLOR_BLACK    0x0000
#define LCD_COLOR_GREY     0xF7DE
#define LCD_COLOR_BLUE     0x001F
#define LCD_COLOR_BLUE2    0x051F
#define LCD_COLOR_RED      0xF800
#define LCD_COLOR_MAGENTA  0xF81F
#define LCD_COLOR_GREEN    0x07E0
#define LCD_COLOR_CYAN     0x7FFF
#define LCD_COLOR_YELLOW   0xFFE0

/* Delays 50 ms and 200 ms for clock 120 MHz */
#define T50ms   1500000
#define T200ms  6000000

typedef struct {
  volatile uint16_t LCD_REG;
  volatile uint16_t LCD_RAM;
} LCD_TypeDef;

/* Memory address of the LCD module, Bank 1 NOR/PSRAM 3 */
#define LCD ((LCD_TypeDef *)0x68000000)

static const font_t *CurrentFont;
static uint16_t TextColor = LCD_COLOR_BLACK;
static uint16_t BackColor = LCD_COLOR_WHITE;

/* Current character line and position, the number of lines, the
   number of characters in a line, line 0 and position 0 offset on
   screen in pixels */
static int Line, Position, TextHeight, TextWidth, XOffset, YOffset;

/** Internal functions **/

static void GPIOconfigure(void) {
  GPIO_InitTypeDef GPIO_InitStruct;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD |
                         RCC_AHB1Periph_GPIOG |
                         RCC_AHB1Periph_GPIOE |
                         RCC_AHB1Periph_GPIOF,
                         ENABLE);

  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;

  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0  | GPIO_Pin_1  | GPIO_Pin_4  |
                             GPIO_Pin_5  | GPIO_Pin_8  | GPIO_Pin_9  |
                             GPIO_Pin_10 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_Init(GPIOD, &GPIO_InitStruct);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource4, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource10, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_FSMC);

  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7  | GPIO_Pin_8  | GPIO_Pin_9  |
                             GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 |
                             GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_Init(GPIOE, &GPIO_InitStruct);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource7 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource8 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource9 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource10 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource11 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource12 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource13 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource14 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource15 , GPIO_AF_FSMC);

  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
  GPIO_Init(GPIOF, &GPIO_InitStruct);
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource0, GPIO_AF_FSMC);

  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
  GPIO_Init(GPIOG, &GPIO_InitStruct);
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource10, GPIO_AF_FSMC);
}

static void FSMCconfigure(void) {
  FSMC_NORSRAMTimingInitTypeDef FSMC_NORSRAMTimingInitStruct;
  FSMC_NORSRAMInitTypeDef       FSMC_NORSRAMInitStruct;

  RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);

  FSMC_NORSRAMTimingInitStruct.FSMC_AddressSetupTime = 3;
  FSMC_NORSRAMTimingInitStruct.FSMC_AddressHoldTime = 0;
  FSMC_NORSRAMTimingInitStruct.FSMC_DataSetupTime = 9;
  FSMC_NORSRAMTimingInitStruct.FSMC_BusTurnAroundDuration = 0;
  FSMC_NORSRAMTimingInitStruct.FSMC_CLKDivision = 0;
  FSMC_NORSRAMTimingInitStruct.FSMC_DataLatency = 0;
  FSMC_NORSRAMTimingInitStruct.FSMC_AccessMode = FSMC_AccessMode_A;

  FSMC_NORSRAMInitStruct.FSMC_Bank = FSMC_Bank1_NORSRAM3;
  FSMC_NORSRAMInitStruct.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
  FSMC_NORSRAMInitStruct.FSMC_MemoryType = FSMC_MemoryType_SRAM;
  FSMC_NORSRAMInitStruct.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
  FSMC_NORSRAMInitStruct.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
  FSMC_NORSRAMInitStruct.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
  FSMC_NORSRAMInitStruct.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
  FSMC_NORSRAMInitStruct.FSMC_WrapMode = FSMC_WrapMode_Disable;
  FSMC_NORSRAMInitStruct.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
  FSMC_NORSRAMInitStruct.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
  FSMC_NORSRAMInitStruct.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
  FSMC_NORSRAMInitStruct.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
  FSMC_NORSRAMInitStruct.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
  FSMC_NORSRAMInitStruct.FSMC_ReadWriteTimingStruct = &FSMC_NORSRAMTimingInitStruct;
  FSMC_NORSRAMInitStruct.FSMC_WriteTimingStruct = &FSMC_NORSRAMTimingInitStruct;

  FSMC_NORSRAMInit(&FSMC_NORSRAMInitStruct);
  FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM3, ENABLE);
}

static uint16_t LCDreadReg(uint16_t reg) {
  /* Write the register index. */
  LCD->LCD_REG = reg;
  /* Read the register value. */
  return LCD->LCD_RAM;
}

static void LCDwriteReg(uint16_t reg, uint16_t value) {
  /* Write the register index. */
  LCD->LCD_REG = reg;
  /* Write the register value. */
  LCD->LCD_RAM = value;
}

static int ControllerConfigure(void) {
  uint16_t lcdid;

  lcdid = LCDreadReg(0x00);
  if (lcdid == 0x9320) { /* TODO: ILI9320 controller not tested */
    /* Initial sequence */
    LCDwriteReg(229, 0x8000); /* Set the internal vcore voltage */
    LCDwriteReg(0,   0x0001); /* Start internal OSC */
    LCDwriteReg(1,   0x0100); /* Set SS and SM bit */
    LCDwriteReg(2,   0x0700); /* Set 1 line inversion */
    LCDwriteReg(3,   0x1030); /* Set GRAM write direction and BGR=1 */
    LCDwriteReg(4,   0x0000); /* Resize register */
    LCDwriteReg(8,   0x0202); /* Set the back porch and front porch */
    LCDwriteReg(9,   0x0000); /* Set non-display area refresh cycle ISC[3:0] */
    LCDwriteReg(10,  0x0000); /* FMARK function */
    LCDwriteReg(12,  0x0000); /* RGB interface setting */
    LCDwriteReg(13,  0x0000); /* Frame marker position */
    LCDwriteReg(15,  0x0000); /* RGB interface polarity */

    /* Power on sequence */
    LCDwriteReg(16, 0x0000); /* SAP, BT[3:0], AP, DSTB, SLP, STB */
    LCDwriteReg(17, 0x0000); /* DC1[2:0], DC0[2:0], VC[2:0] */
    LCDwriteReg(18, 0x0000); /* VREG1OUT voltage */
    LCDwriteReg(19, 0x0000); /* VDV[4:0] for VCOM amplitude */
    Delay(T200ms);           /* Discharge capacitor power voltage */
    LCDwriteReg(16, 0x17B0); /* SAP, BT[3:0], AP, DSTB, SLP, STB */
    LCDwriteReg(17, 0x0137); /* DC1[2:0], DC0[2:0], VC[2:0] */
    Delay(T50ms);            /* Delay 50 ms */
    LCDwriteReg(18, 0x0139); /* VREG1OUT voltage */
    Delay(T50ms);            /* Delay 50 ms */
    LCDwriteReg(19, 0x1d00); /* VDV[4:0] for VCOM amplitude */
    LCDwriteReg(41, 0x0013); /* VCM[4:0] for VCOMH */
    Delay(T50ms);            /* Delay 50 ms */
    LCDwriteReg(32, 0x0000); /* GRAM horizontal address */
    LCDwriteReg(33, 0x0000); /* GRAM vertical address */

    /* Adjust the gamma curve */
    LCDwriteReg(48, 0x0006);
    LCDwriteReg(49, 0x0101);
    LCDwriteReg(50, 0x0003);
    LCDwriteReg(53, 0x0106);
    LCDwriteReg(54, 0x0b02);
    LCDwriteReg(55, 0x0302);
    LCDwriteReg(56, 0x0707);
    LCDwriteReg(57, 0x0007);
    LCDwriteReg(60, 0x0600);
    LCDwriteReg(61, 0x020b);

    /* Set GRAM area */
    LCDwriteReg(80,  0x0000);               /* Horizontal GRAM start address */
    LCDwriteReg(81,  LCD_PIXEL_HEIGHT - 1); /* Horizontal GRAM End address */
    LCDwriteReg(82,  0x0000);               /* Vertical GRAM start address */
    LCDwriteReg(83,  LCD_PIXEL_WIDTH - 1);  /* Vertical GRAM end address */
    LCDwriteReg(96,  0x2700);               /* Gate scan line */
    LCDwriteReg(97,  0x0001);               /* NDL,VLE, REV */
    LCDwriteReg(106, 0x0000);               /* Set scrolling line */

    /* Partial display control */
    LCDwriteReg(128, 0x0000);
    LCDwriteReg(129, 0x0000);
    LCDwriteReg(130, 0x0000);
    LCDwriteReg(131, 0x0000);
    LCDwriteReg(132, 0x0000);
    LCDwriteReg(133, 0x0000);

    /* Panel control */
    LCDwriteReg(144, 0x0010);
    LCDwriteReg(146, 0x0000);
    LCDwriteReg(147, 0x0003);
    LCDwriteReg(149, 0x0110);
    LCDwriteReg(151, 0x0000);
    LCDwriteReg(152, 0x0000);

    /* Set GRAM write direction and BGR == 1 */
    /* I/D=11 (horizontal increment, vertical increment) */
    /* AM=1 (address is updated in vertical writing direction) */
    LCDwriteReg(3, 0x1038);

    LCDwriteReg(7, 0x0173); /* 262k colors and display on */

    return 0;
  }
  else if (lcdid == 0x9325) { /* ILI9325 controller */
    /* Initial sequence */
    LCDwriteReg(0,  0x0001); /* Start internal OSC. */
    LCDwriteReg(1,  0x0100); /* Set SS and SM bit */
    LCDwriteReg(2,  0x0700); /* Set 1 line inversion */
    LCDwriteReg(3,  0x1018); /* Set GRAM write direction and BGR=1 */
    LCDwriteReg(4,  0x0000); /* Resize register */
    LCDwriteReg(8,  0x0202); /* Set the back porch and front porch */
    LCDwriteReg(9,  0x0000); /* Set non-display area refresh cycle ISC[3:0] */
    LCDwriteReg(10, 0x0000); /* FMARK function */
    LCDwriteReg(12, 0x0000); /* RGB interface setting */
    LCDwriteReg(13, 0x0000); /* Frame marker position */
    LCDwriteReg(15, 0x0000); /* RGB interface polarity */

    /* Power on sequence */
    LCDwriteReg(16, 0x0000); /* SAP, BT[3:0], AP, DSTB, SLP, STB */
    LCDwriteReg(17, 0x0000); /* DC1[2:0], DC0[2:0], VC[2:0] */
    LCDwriteReg(18, 0x0000); /* VREG1OUT voltage */
    LCDwriteReg(19, 0x0000); /* VDV[4:0] for VCOM amplitude */
    Delay(T200ms);           /* Discharge capacitor power voltage */
    LCDwriteReg(16, 0x17B0); /* SAP, BT[3:0], AP, DSTB, SLP, STB */
    LCDwriteReg(17, 0x0137); /* DC1[2:0], DC0[2:0], VC[2:0] */
    Delay(T50ms);            /* Delay 50 ms */
    LCDwriteReg(18, 0x0139); /* VREG1OUT voltage */
    Delay(T50ms);            /* Delay 50 ms */
    LCDwriteReg(19, 0x1d00); /* VDV[4:0] for VCOM amplitude */
    LCDwriteReg(41, 0x0013); /* VCM[4:0] for VCOMH */
    Delay(T50ms);            /* Delay 50 ms */
    LCDwriteReg(32, 0x0000); /* GRAM horizontal address */
    LCDwriteReg(33, 0x0000); /* GRAM vertical address */

    /* Adjust the gamma curve */
    LCDwriteReg(48, 0x0007);
    LCDwriteReg(49, 0x0302);
    LCDwriteReg(50, 0x0105);
    LCDwriteReg(53, 0x0206);
    LCDwriteReg(54, 0x0808);
    LCDwriteReg(55, 0x0206);
    LCDwriteReg(56, 0x0504);
    LCDwriteReg(57, 0x0007);
    LCDwriteReg(60, 0x0105);
    LCDwriteReg(61, 0x0808);

    /* Set GRAM area */
    LCDwriteReg(80, 0x0000);               /* Horizontal GRAM start address */
    LCDwriteReg(81, LCD_PIXEL_HEIGHT - 1); /* Horizontal GRAM end address */
    LCDwriteReg(82, 0x0000);               /* Vertical GRAM start address */
    LCDwriteReg(83, LCD_PIXEL_WIDTH - 1);  /* Vertical GRAM end address */
    LCDwriteReg(96,  0x2700); /* Gate scan line (GS=0, scan direction is G1~G320) */
    LCDwriteReg(97,  0x0001); /* NDL,VLE, REV */
    LCDwriteReg(106, 0x0000); /* Set scrolling line */

    /* Partial display control */
    LCDwriteReg(128, 0x0000);
    LCDwriteReg(129, 0x0000);
    LCDwriteReg(130, 0x0000);
    LCDwriteReg(131, 0x0000);
    LCDwriteReg(132, 0x0000);
    LCDwriteReg(133, 0x0000);

    /* Panel control */
    LCDwriteReg(144, 0x0010);
    LCDwriteReg(146, 0x0000);
    LCDwriteReg(147, 0x0003);
    LCDwriteReg(149, 0x0110);
    LCDwriteReg(151, 0x0000);
    LCDwriteReg(152, 0x0000);

    /* Set GRAM write direction and BGR = 1 */
    /* I/D=11 (horizontal increment, vertical increment) */
    /* AM=1 (address is updated in vertical writing direction) */
    LCDwriteReg(3, 0x1038);

    LCDwriteReg(7, 0x0133); /* 262k colors and display on */

    return 0;
  }
  else {
    return -1;
  }
}

static void LCDwriteRAMprepare(void) {
  LCD->LCD_REG = 34;
}

static void LCDwriteRAM(uint16_t RGBcode) {
  LCD->LCD_RAM = RGBcode;
}

static void LCDsetFont(const font_t *font) {
  CurrentFont = font;
  TextHeight = LCD_PIXEL_HEIGHT / CurrentFont->height;
  TextWidth  = LCD_PIXEL_WIDTH  / CurrentFont->width;
  XOffset = (LCD_PIXEL_HEIGHT - TextHeight * CurrentFont->height) / 2;
  YOffset = (LCD_PIXEL_WIDTH  - TextWidth  * CurrentFont->width)  / 2;
}

static void LCDsetColors(uint16_t text, uint16_t back) {
  TextColor = text;
  BackColor = back;
}

static void LCDsetPosition(uint16_t xpos, uint16_t ypos) {
  LCDwriteReg(32, xpos);
  LCDwriteReg(33, ypos);
}

static void LCDdrawChar(unsigned c) {
  uint16_t const *p;
  uint16_t x, y, w;
  int      i, j;

  x = XOffset + CurrentFont->height * Line;
  y = YOffset + CurrentFont->width  * Position;
  p = &CurrentFont->table[(c - FIRST_CHAR) * CurrentFont->height];
  for (i = 0; i < CurrentFont->height; ++i, ++x) {
    LCDsetPosition(x, y);
    LCDwriteRAMprepare();
    for (j = 0, w = p[i]; j < CurrentFont->width; ++j, w >>= 1) {
      LCDwriteRAM(w & 1 ? TextColor : BackColor);
    }
  }
}

/** Public interface implementation **/

int LCDconfigure() {
  GPIOconfigure();
  FSMCconfigure();
  Delay(T50ms);
  if (ControllerConfigure() < 0)
    return -1;
  LCDsetFont(&LCD_DEFAULT_FONT);
  LCDsetColors(LCD_COLOR_WHITE, LCD_COLOR_BLUE);
  LCDclear();
  return 0;
}

void LCDclear() {
  int index;

  LCDsetPosition(0, 0); /* left top corner */
  LCDwriteRAMprepare();
  for (index = 0; index < LCD_PIXEL_WIDTH * LCD_PIXEL_HEIGHT; ++index)
    LCD->LCD_RAM = BackColor;
  LCDgoto(0, 0);
}

void LCDgoto(int textLine, int charPos) {
  Line = textLine;
  Position = charPos;
}

void LCDputchar(char c) {
  if (c == '\n')
    LCDgoto(Line + 1, 0); /* line feed */
  else if (c == '\r')
    LCDgoto(Line, 0); /* carriage return */
  else if (c == '\t')
    LCDgoto(Line, (Position + 8) & ~7); /* tabulator */
  else {
    if (c >= FIRST_CHAR && c <= LAST_CHAR &&
        Line >= 0 && Line < TextHeight &&
        Position >= 0 && Position < TextWidth) {
      LCDdrawChar(c);
    }
    LCDgoto(Line, Position + 1);
  }
}

void LCDputcharWrap(char c) {
  if (Position >= TextWidth)
    LCDputchar('\n'); /* There is no room for the next character. */
  LCDputchar(c);
}
