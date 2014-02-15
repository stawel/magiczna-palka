/* Blink without Delay
 
 Turns on and off a light emitting diode(LED) connected to a digital  
 pin, without using the delay() function.  This means that other code
 can run at the same time without being interrupted by the LED code.
 
 The circuit:
 * LED attached from pin 13 to ground.
 * Note: on most Arduinos, there is already an LED on the board
 that's attached to pin 13, so no hardware is needed for this example.
 
 
 created 2005
 by David A. Mellis
 modified 8 Feb 2010
 by Paul Stoffregen
 
 This example code is in the public domain.

 
 http://www.arduino.cc/en/Tutorial/BlinkWithoutDelay
 */

void blink(){}
// constants won't change. Used here to 
// set pin numbers:
const int ledPin =  13;      // the number of the LED pin
const int inPin1 =  9;      // the number of the LED pin
const int inPin2 =  10;      // the number of the LED pin
const int inPin3 =  11;      // the number of the LED pin
const int inLowPin =  12;      // the number of the LED pin
const int speaker1Pin =  A0;      // the number of the LED pin
const int speaker1LowPin =  A1;      // the number of the LED pin
const int speaker2Pin =  A2;      // the number of the LED pin
const int speaker2LowPin =  A3;      // the number of the LED pin
const int speaker3Pin =  A4;      // the number of the LED pin
const int speaker3LowPin =  A5;      // the number of the LED pin
const int speaker4Pin =  0;      // the number of the LED pin
const int speaker4LowPin =  1;      // the number of the LED pin

// Variables will change:
int ledState = LOW;             // ledState used to set the LED
long previousMillis = 0;        // will store last time LED was updated

// the follow variables is a long because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long interval = 1000;           // interval at which to blink (milliseconds)

void setup() {
  // set the digital pin as output:

  interrupts(); 
  PCICR |= (1 << PCIE0); // enable PCINT0
  EICRA |= (1 << ISC11) | (1 << ISC01) ; //falling edge
  PCMSK0 |= (1 << PCINT1); 
  
//  attachInterrupt(inPin1, blink, FALLING);

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

#define DELAY 7
#include <util/delay_basic.h>

uint8_t spmap[]= {speaker1Pin,speaker2Pin,speaker3Pin,speaker1Pin};



uint8_t dmax = 65+10;//60-20;//29
uint8_t dmin = 65-10;//60-5;//30;//30-1;//29


uint8_t d = dmax;


void mdelay0(uint8_t dly) 
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

void mdelay2(uint8_t dly) 
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

void mdelay3(uint8_t dly) 
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
  asm("nop"); 
  _delay_loop_1(dly);
}

void mdelay1(uint8_t dly) 
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
  asm("nop"); 
  _delay_loop_1(dly);
}


void speaker0(uint8_t a,uint8_t b)
{
    uint8_t d2 =64;//65;
    uint8_t e = d2;
      PORTD = a;      mdelay0(d2);
      PORTD = b;      mdelay0(d2);
      PORTD = 0;      mdelay0(e);

      PORTD = a;      mdelay0(d2);
      PORTD = b;      mdelay0(d2);
      PORTD = a;      mdelay0(d2);
      PORTD = 0;      
}

void speaker2(uint8_t a,uint8_t b)
{
    uint8_t d2 = 64;//65;
    uint8_t e=d2;
      PORTC = a;      mdelay2(d2);
      PORTC = b;      mdelay2(d2);
      PORTC = 0;      mdelay2(e);

      PORTC = a;      mdelay2(d2);
      PORTC = b;      mdelay2(d2);
      PORTC = a;      mdelay2(d2);
     PORTC = 0;      
}

void speaker1(uint8_t a,uint8_t b)
{
    uint8_t d2 = 64;//65;
    uint8_t e=d2;
      PORTC = a;      mdelay1(d2);
      PORTC = b;      mdelay1(d2);
      PORTC = 0;      mdelay1(e);

      PORTC = a;      mdelay1(d2);
      PORTC = b;      mdelay1(d2);
      PORTC = a;      mdelay1(d2);
     PORTC = 0;      
}

void speaker3(uint8_t a,uint8_t b)
{
    uint8_t d2 = 64;//65;
    uint8_t e=d2;
      PORTC = a;      mdelay3(d2);
      PORTC = b;      mdelay3(d2);
      PORTC = 0;      mdelay3(e);

      PORTC = a;      mdelay3(d2);
      PORTC = b;      mdelay3(d2);
      PORTC = a;      mdelay3(d2);
     PORTC = 0;      
}



ISR(PCINT0_vect) // same as above for PCINT1
{
  if(digitalRead(inPin1))
    return;
    
  byte saveSREG = SREG;
  noInterrupts();
      
    volatile uint8_t j;
    uint8_t a,b; 
    uint8_t witch = (1&digitalRead(inPin2)) + ((1&digitalRead(inPin3)) << 1);
    switch(witch) {
      case 0: a=1,b=2; break;
      case 1: a=4,b=8; break;
      case 2: a=16,b=32; break;
      case 3: a=1,b=2; break;
    }
    
    digitalWrite(ledPin, HIGH);
    switch(witch) {
     case 3: speaker0(a,b); break;
     case 2: speaker2(a,b); break;
     case 0: speaker3(a,b); break;
     case 1: speaker1(a,b); break;
    }
    
    digitalWrite(ledPin, LOW);
    static uint8_t f=35;
    if(!--f) {
      d--;
      f=15;
    }
    if(d<dmin) d = dmax;
    
    for(int k=0;k<200;k++)
      _delay_loop_1(200);
    SREG = saveSREG;
}

void loop()
{
/*  while(1) {
    blink();
    delay(1);
    while(digitalRead(inPin));
  }
*/
}

