#include <board_def.h>
#include <delay.h>
#include <lcd.h>

#define DATA_PORT  xcat(GPIO, LCD_DATA_GPIO_N)
#define DATA_RCC   xcat(RCC_APB2Periph_GPIO, LCD_DATA_GPIO_N)
#define D0_D7      (0xffU << LCD_D0_PIN_N)

#define CTRL_PORT  xcat(GPIO, LCD_CTRL_GPIO_N)
#define CTRL_RCC   xcat(RCC_APB2Periph_GPIO, LCD_CTRL_GPIO_N)
#define RS_LINE    xcat(GPIO_Pin_, LCD_RS_PIN_N)
#define E_LINE     xcat(GPIO_Pin_, LCD_E_PIN_N)
#define RW_LINE    xcat(GPIO_Pin_, LCD_RW_PIN_N)

/* HD44780 wymaga opóźnień 40 ns i 230 ns. Wartości dobrane tak, aby
   LCD działał nawet dla maksymalnej częstotliwości taktowania rdzenia
   72 MHz. W razie problemów należy spróbować je zwiększyć. */
#define T40   1
#define T230  5

/* Maksymalna liczba prób odczytu statusu w oczekiwaniu,
   aż przestanie być BUSY. */
#define MAX_ATTEMPTS  30000
#define BUSY_STATUS   0x80

/* Organizacja wyświetlacza 2 x 16 znaków */
#define SCREEN_HEIGHT  2
#define SCREEN_WIDTH   16

/* Adresy pierwszej komórki pamięci dla kolejnych wierszy */
static uint8_t x_translation[4] = {0, 0x40, 0x20, 0x60};

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
  Delay(T40);
  E(1);
  Delay(T230);
  data = DataIn();
  E(0);
  return data;
}

static void WriteByte(uint8_t data) {
  RW(0);          /* Najpierw wystaw sygnał WRITE. */
  DataLinesOut(); /* Potem ustaw jako wyjścia. */
  Delay(T40);
  E(1);
  DataOut(data);
  Delay(T230);
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
   a y to numer kolumny w wierszu liczony od lewej */
static int x, y;

int LCDconfigure() {
  GPIO_InitTypeDef GPIO_InitStruct;
  uint8_t          status;

  RCC_APB2PeriphClockCmd(CTRL_RCC | DATA_RCC, ENABLE);

  /* Ustaw poziomy, które pojawią się na wyprowadzeniach po ich
     skonfigurowaniu. Na lini E dobrze jest zacząć od poziomu
     niskiego. Na RS i RW poziom niski, bo zaczynamy od wysyłania
     instrukcji. */
  E(0);
  RS(0);
  RW(0);

  /* Konfiguruj wyprowadzenia, które są wyjściami.
     Czas narastania i opadania dla linii E <= 20 ns. */
  GPIO_InitStruct.GPIO_Pin = E_LINE | RS_LINE | RW_LINE;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(CTRL_PORT, &GPIO_InitStruct);

  /* Zerowanie programowe w zasadzie nie jest konieczne.
     Początkowe opóźnienie 40 ms już odczekano w funkcji main. */
  WriteByte(0x38);
  Delay(80000);    /* Czekaj 4,1 ms. */
  WriteByte(0x38);
  Delay(2000);     /* Czekaj 100 us. */
  WriteByte(0x38);
  Delay(2000);     /* Czekaj 100 us. */

  /* Status można czytać dopiero po wyzerowaniu sterownika LCD. */
  status = ReadByte();
  if (status & BUSY_STATUS)
    return -1;

  WriteInstruction(0x38); /* 8-bitowy interfejs, 2 linie, znaki 5x8 */
  WriteInstruction(0x08); /* wyświetlanie wyłączone */
  WriteInstruction(0x01); /* wypełnienie RAM znakami spacji */
  WriteInstruction(0x02); /* kursor w pozycji początkowej */
  WriteInstruction(0x06); /* zwiększanie adresu po zapisie do RAM */
  WriteInstruction(0x14); /* przesuwanie kursora w prawo */
  WriteInstruction(0x0c); /* wyświetlanie włączone, bez kursora */

  return 0;
}

void LCDclear() {
  WriteInstruction(0x02); /* kursor w pozycji początkowej */
  WriteInstruction(0x01); /* wypełnienie RAM znakami spacji */
}

void LCDgoto(int textLine, int charPos) {
  x = textLine;
  y = charPos;

  WriteInstruction(0x80 | (x_translation[x] + y));
}

void LCDputchar(char c) {
  if (c == '\n')
    LCDgoto(x + 1, 0); /* przejście do następnego wiersza */
  else if (c == '\r')
    LCDgoto(x, 0); /* powrót na początek wiersza */
  else if (c == '\t')
    LCDgoto(x, (y + 8) & ~7); /* tabulator */
  else if (x >= 0 && x < SCREEN_HEIGHT &&
           y >= 0 && y < SCREEN_WIDTH) {
    WriteDataToRam(c);
    ++y; /* Wołanie LCDgoto jest zbędne. */
  }
}

void LCDputcharWrap(char c) {
  if (y >= SCREEN_WIDTH)
    LCDputchar('\n'); /* Nie ma miejsca na następny znak, zawiń. */
  LCDputchar(c);
}
