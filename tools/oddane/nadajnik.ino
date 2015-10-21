
const int ledPin = 13;      // the number of the LED pin
const int inPin1 = 9;
const int inPin2 = 10;
const int inPin3 = 11;
const int inLowPin = 12;
const int speaker1Pin = A0;
const int speaker1LowPin = A1;
const int speaker2Pin = A2;
const int speaker2LowPin = A3;
const int speaker3Pin = A4;
const int speaker3LowPin = A5;
const int speaker4Pin = 0;
const int speaker4LowPin = 1;

void setup() {
    interrupts();
    PCICR |= (1 << PCIE0); // enable PCINT0
    EICRA |= (1 << ISC11) | (1 << ISC01) ; //falling edge
    PCMSK0 |= (1 << PCINT1);

    pinMode(ledPin, OUTPUT);
    pinMode(inPin1, INPUT);
    pinMode(inPin2, INPUT);
    pinMode(inPin3, INPUT);
    digitalWrite(inPin1,1);
    digitalWrite(inPin2,1);
    digitalWrite(inPin3,1);
    pinMode(inLowPin, OUTPUT);
    digitalWrite(inLowPin,0);

    pinMode(speaker1Pin, OUTPUT);
    pinMode(speaker1LowPin, OUTPUT);
    digitalWrite(speaker1Pin,0);
    digitalWrite(speaker1LowPin,0);
    pinMode(speaker2Pin, OUTPUT);
    pinMode(speaker2LowPin, OUTPUT);
    digitalWrite(speaker2Pin,0);
    digitalWrite(speaker2LowPin,0);
    pinMode(speaker3Pin, OUTPUT);
    pinMode(speaker3LowPin, OUTPUT);
    digitalWrite(speaker3Pin,0);
    digitalWrite(speaker3LowPin,0);

    pinMode(speaker4Pin, OUTPUT);
    pinMode(speaker4LowPin, OUTPUT);
    digitalWrite(speaker4Pin,0);
    digitalWrite(speaker4LowPin,0);
}

#include <util/delay_basic.h>


void _mdelay(uint8_t dly)
{
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    _delay_loop_1(dly);
}

inline void mdelay0(uint8_t dly){    _mdelay(dly); }
inline void mdelay1(uint8_t dly){    _mdelay(dly); }
inline void mdelay2(uint8_t dly){    _mdelay(dly); }
inline void mdelay3(uint8_t dly){    _mdelay(dly); }

void speaker0(uint8_t a, uint8_t b)
{
    uint8_t d = 64;
    PORTD = a;      mdelay0(d);
    PORTD = b;      mdelay0(d);
    PORTD = 0;      mdelay0(d);

    PORTD = a;      mdelay0(d);
    PORTD = b;      mdelay0(d);
    PORTD = a;      mdelay0(d);
    PORTD = 0;
}

void speaker2(uint8_t a, uint8_t b)
{
    uint8_t d = 64;
    PORTC = a;      mdelay2(d);
    PORTC = b;      mdelay2(d);
    PORTC = 0;      mdelay2(d);

    PORTC = a;      mdelay2(d);
    PORTC = b;      mdelay2(d);
    PORTC = a;      mdelay2(d);
    PORTC = 0;
}

void speaker1(uint8_t a, uint8_t b)
{
    uint8_t d = 64;
    PORTC = a;      mdelay1(d);
    PORTC = b;      mdelay1(d);
    PORTC = 0;      mdelay1(d);

    PORTC = a;      mdelay1(d);
    PORTC = b;      mdelay1(d);
    PORTC = a;      mdelay1(d);
    PORTC = 0;
}

void speaker3(uint8_t a, uint8_t b)
{
    uint8_t d = 64;
    PORTC = a;      mdelay3(d);
    PORTC = b;      mdelay3(d);
    PORTC = 0;      mdelay3(d);

    PORTC = a;      mdelay3(d);
    PORTC = b;      mdelay3(d);
    PORTC = a;      mdelay3(d);
    PORTC = 0;
}



ISR(PCINT0_vect)
{
    if(digitalRead(inPin1))
        return;

    byte saveSREG = SREG;
    noInterrupts();

    uint8_t a, b;
    uint8_t witch = (1 & digitalRead(inPin2)) + ((1 & digitalRead(inPin3)) << 1);
    switch(witch) {
        case 0: a = 1, b = 2;  break;
        case 1: a = 4, b = 8;  break;
        case 2: a = 16,b = 32; break;
        case 3: a = 1, b = 2;  break;
    }

    digitalWrite(ledPin, HIGH);
    switch(witch) {
        case 3: speaker0(a, b); break;
        case 2: speaker2(a, b); break;
        case 0: speaker3(a, b); break;
        case 1: speaker1(a, b); break;
    }

    digitalWrite(ledPin, LOW);

    for(int k = 0; k < 200; k++) {
        _delay_loop_1(200);
    }
    SREG = saveSREG;
}

void loop()
{
}

