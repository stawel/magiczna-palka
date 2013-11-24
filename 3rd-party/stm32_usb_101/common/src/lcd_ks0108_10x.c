#include <board_def.h>
#include <delay.h>
#include <font5x8.h>
#include <lcd.h>

#define DATA_PORT  xcat(GPIO, LCD_DATA_GPIO_N)
#define DATA_RCC   xcat(RCC_APB2Periph_GPIO, LCD_DATA_GPIO_N)
#define D0_D7      (0xffU << LCD_D0_PIN_N)

#define CTRL_PORT  xcat(GPIO, LCD_CTRL_GPIO_N)
#define CTRL_RCC   xcat(RCC_APB2Periph_GPIO, LCD_CTRL_GPIO_N)
#define RS_LINE    xcat(GPIO_Pin_, LCD_RS_PIN_N)
#define E_LINE     xcat(GPIO_Pin_, LCD_E_PIN_N)
#define RW_LINE    xcat(GPIO_Pin_, LCD_RW_PIN_N)
#define CS1_LINE   xcat(GPIO_Pin_, LCD_CS1_PIN_N)
#define CS2_LINE   xcat(GPIO_Pin_, LCD_CS2_PIN_N)

/* Początkowe opóźnienie potrzebne po resecie - dobrane
   eksperymentalnie, ale z dużym zapasem. */
#define INIT_DELAY  600

/* KS0108 wymaga opóźnień 140 ns i 450 ns. Nawet dla maksymalnej
   częstotliwości taktowania rdzenia 72 MHz powinno wystarczyć
      #define T140 2
      #define T450 7
   Mój egzemplarz LCD działa z zerowymi wartościami.
   W razie problemów należy spróbować je zwiększyć. */
#define T140  0
#define T450  0

/* Maksymalna liczba prób odczytu statusu w oczekiwaniu,
   aż przestanie być BUSY. */
#define MAX_ATTEMPTS      1000

#define DISPLAY_ON        0x3F
#define SET_ADDRESS_Y     0x40
#define SET_PAGE_X        0xB8
#define SET_START_LINE_Z  0xC0
#define BUSY_STATUS       0x80

/* Organizacja wyświetlacza 64 x 128 pikseli - 2 segmenty */
#define X_SEGMENTS        1
#define Y_SEGMENTS        2
#define PAGE_X_BITS       3
#define ADDRESS_Y_BITS    6
#define PAGE_X_MASK       ((1 << PAGE_X_BITS) - 1)
#define ADDRESS_Y_MASK    ((1 << ADDRESS_Y_BITS) - 1)
#define SCREEN_WIDTH      (Y_SEGMENTS * (1 << ADDRESS_Y_BITS))
#define SCREEN_HEIGHT     (X_SEGMENTS * (1 << PAGE_X_BITS))

/* Prymitywy sterujące liniami LCD. Operują bezpośrednio na
   rejestrach GPIO. Standard Peripherals Library nie dostarcza
   atomowej operacji modyfikacji kilku bitów GPIO, ani szybkiej
   operacji zmiany kierunku kilku linii GPIO. Korzystamy
   bezpośrednio z CMSIS. Dobry kompilator potrafi rozwinąć te
   funkcje w miejscu wołania i zoptymalizować instrukcje
   warunkowe, jeśli argument jest stałą. */

#define SET_BITS(x) ((uint32_t)(x))
#define CLR_BITS(x) ((uint32_t)(x) << 16)

static void E(int x) {
  if (x)
    CTRL_PORT->BSRR = E_LINE;
  else
    CTRL_PORT->BRR  = E_LINE;
}

static void RS(int x) {
  if (x)
    CTRL_PORT->BSRR = RS_LINE;
  else
    CTRL_PORT->BRR  = RS_LINE;
}

static void RW(int x) {
  if (x)
    CTRL_PORT->BSRR = RW_LINE;
  else
    CTRL_PORT->BRR  = RW_LINE;
}

/* Dla tego LCD aktywnym poziomem CS jest poziom wysoki. */

static void ChipUnselect() {
  CTRL_PORT->BSRR = CLR_BITS(CS1_LINE | CS2_LINE);
}

static void ChipSelect(int x, int y) {
  if (x == 0 && y == 0)
    CTRL_PORT->BSRR = SET_BITS(CS1_LINE) | CLR_BITS(CS2_LINE);
  else if (x == 0 && y == 1)
    CTRL_PORT->BSRR = SET_BITS(CS2_LINE) | CLR_BITS(CS1_LINE);
  else
    ChipUnselect();
}

static void DataOut(uint8_t x) {
  DATA_PORT->BSRR = CLR_BITS(~((uint32_t)x << LCD_D0_PIN_N) & D0_D7) |
                    SET_BITS( ((uint32_t)x << LCD_D0_PIN_N) & D0_D7);
}

static uint8_t DataIn(void) {
  return (DATA_PORT->IDR & D0_D7) >> LCD_D0_PIN_N;
}

/* Funkcje DataLinesIn i DataLinesOut nie rekonfigurują portu
   atomowo. Żaden inny proces lub procedura obsługi przerwania
   NIE MOGĄ modyfikować rejestru CRL lub CRH dla DATA_PORT. */

static void DataLinesIn(void) {
  uint32_t r;

  /* Rezystory podciągające wymuszają, że status odczytany przy braku
     LCD będzie oznaczał brak gotowości. 0x88888888U oznacza wejście
     z rezystorem podciągającym do VCC lub ściągającym do masy. */
  DATA_PORT->BSRR = SET_BITS(D0_D7);
  if (LCD_D0_PIN_N < 8) {
    r = DATA_PORT->CRL;
    r &= ~(0xffffffffU << (4 * LCD_D0_PIN_N));
    r |=   0x88888888U << (4 * LCD_D0_PIN_N);
    DATA_PORT->CRL = r;
  }
  if (LCD_D0_PIN_N > 0) {
    r = DATA_PORT->CRH;
    r &= ~(0xffffffffU >> (32 - 4 * LCD_D0_PIN_N));
    r |=   0x88888888U >> (32 - 4 * LCD_D0_PIN_N);
    DATA_PORT->CRH = r;
  }
}

static void DataLinesOut(void) {
  uint32_t r;

  /* 0x11111111U oznacza wyjście przeciwsobne 10 MHz. */
  if (LCD_D0_PIN_N < 8) {
    r = DATA_PORT->CRL;
    r &= ~(0xffffffffU << (4 * LCD_D0_PIN_N));
    r |=   0x11111111U << (4 * LCD_D0_PIN_N);
    DATA_PORT->CRL = r;
  }
  if (LCD_D0_PIN_N > 0) {
    r = DATA_PORT->CRH;
    r &= ~(0xffffffffU >> (32 - 4 * LCD_D0_PIN_N));
    r |=   0x11111111U >> (32 - 4 * LCD_D0_PIN_N);
    DATA_PORT->CRH = r;
  }
}

/** Podstawowe operacje **/

static uint8_t ReadByte(void) {
  uint8_t data;

  DataLinesIn(); /* Najpierw ustaw jako wejścia. */
  RW(1);         /* Potem wystaw sygnał READ. */
  Delay(T140);
  E(1);
  Delay(T450);
  data = DataIn();
  E(0);
  return data;
}

static void WriteByte(uint8_t data) {
  RW(0);          /* Najpierw wystaw sygnał WRITE. */
  DataLinesOut(); /* Potem ustaw jako wyjścia. */
  Delay(T140);
  E(1);
  DataOut(data);
  Delay(T450);
  E(0);
}

static uint8_t Status(void) {
  int     i;
  uint8_t status;

  RS(0); /* instrukcja */
  i = 0;
  do
    status = ReadByte();
  while (++i < MAX_ATTEMPTS && (status & BUSY_STATUS));

  return status;
}

static void WriteInstruction(uint8_t data) {
  if (!(Status() & BUSY_STATUS))
    /* W funkcji Status zostało ustawione 0 na linii RS. */
    WriteByte(data);
}

static void WriteDataToRam(uint8_t data) {
  if (!(Status() & BUSY_STATUS)) {
    RS(1); /* dane */
    WriteByte(data);
  }
}

/** Implementacja interfejsu **/

/* Bieżące położenie "kursora", x to numer wiersza liczony od góry,
   a y to numer piksela w wierszu liczony od lewej */
static int x, y;
static void InternalGoTo(int, int);

int LCDconfigure() {
  int              sx, sy;
  GPIO_InitTypeDef GPIO_InitStruct;
  uint8_t          status;

  RCC_APB2PeriphClockCmd(CTRL_RCC | DATA_RCC, ENABLE);

  /* Ustaw poziomy, które pojawią się na wyprowadzeniach po ich
     skonfigurowaniu. Na lini E dobrze jest zacząć od poziomu
     niskiego. Na RS jest to nieistotne, bo jest ustawiana przed
     każdą operacją.*/
  E(0);
  RS(0);
  ChipUnselect();

  /* Linie danych konfiguruj początkowo jako wejścia. */
  DataLinesIn();
  RW(1);

  /* I dopiero teraz konfiguruj wyprowadzenia, które są wyjściami.
     Czas narastania i opadania dla linii E <= 25 ns. */
  GPIO_InitStruct.GPIO_Pin = E_LINE | RS_LINE | RW_LINE |
                             CS1_LINE | CS2_LINE;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(CTRL_PORT, &GPIO_InitStruct);

  /* Trzeba odczekać, zanim zacznie się wysyłać polecenia. */
  Delay(INIT_DELAY);

  /* Sprawdź, czy LCD jest podłączony? */
  for (sx = 0; sx < X_SEGMENTS; ++sx) {
    for (sy = 0; sy < Y_SEGMENTS; ++sy) {
      ChipSelect(sx, sy);
      status = ReadByte();
      ChipUnselect();
      if (status & BUSY_STATUS)
        return -1;
    }
  }

  /* Aktywuj wyświetlacz. */
  LCDclear();
  for (sx = 0; sx < X_SEGMENTS; ++sx) {
    for (sy = 0; sy < Y_SEGMENTS; ++sy) {
      ChipSelect(sx, sy);
      WriteInstruction(DISPLAY_ON);
      ChipUnselect();
    }
  }

  return 0;
}

void LCDclear() {
  int ix, iy, sx, sy;

  x = y = 0;

  for (sx = 0; sx < X_SEGMENTS; ++sx) {
    for (sy = 0; sy < Y_SEGMENTS; ++sy) {
      ChipSelect(sx, sy);
      WriteInstruction(SET_START_LINE_Z);
      for (ix = 0; ix <= PAGE_X_MASK; ++ix) {
        WriteInstruction(SET_PAGE_X | ix);
        WriteInstruction(SET_ADDRESS_Y);
        for (iy = 0; iy <= ADDRESS_Y_MASK; ++iy)
          WriteDataToRam(0);
      }
      WriteInstruction(SET_PAGE_X);
      WriteInstruction(SET_ADDRESS_Y);
      ChipUnselect();
    }
  }
}

void LCDgoto(int textLine, int charPos) {
  InternalGoTo(textLine, charPos * (CHAR_WIDTH + 1));
}

void InternalGoTo(int new_x, int new_y) {
  x = new_x;
  y = new_y;
  ChipSelect(x >> PAGE_X_BITS, y >> ADDRESS_Y_BITS);
  WriteInstruction(SET_PAGE_X | (x & PAGE_X_MASK));
  WriteInstruction(SET_ADDRESS_Y | (y & ADDRESS_Y_MASK));
  ChipUnselect();
}

void LCDputchar(char c) {
  int i;

  if (c == '\n')
    LCDgoto(x + 1, 0); /* przejście do następnego wiersza */
  else if (c == '\r')
    LCDgoto(x, 0); /* powrót na początek wiersza */
  else if (c == '\t')
    LCDgoto(x, (y / (CHAR_WIDTH + 1) + 8) & ~7); /* tabulator */
  else if (c >= FIRST_CHAR && c <= LAST_CHAR) {
    for (i = 0; i <= CHAR_WIDTH; ++i) {
      if (x >= 0 && x < SCREEN_HEIGHT &&
          y >= 0 && y < SCREEN_WIDTH) {
        if ((y & ADDRESS_Y_MASK) == 0)
          InternalGoTo(x, y); /* zmiana segmentu */
        ChipSelect(x >> PAGE_X_BITS, y >> ADDRESS_Y_BITS);
        if (i < CHAR_WIDTH)
          WriteDataToRam(font5x8[(unsigned)c - FIRST_CHAR][i]);
        else
          WriteDataToRam(0);
        ChipUnselect();
      }
      ++y;
    }
  }
}

void LCDputcharWrap(char c) {
  if (y > SCREEN_WIDTH - CHAR_WIDTH)
    LCDputchar('\n'); /* Nie ma miejsca na następny znak, zawiń. */
  LCDputchar(c);
}
