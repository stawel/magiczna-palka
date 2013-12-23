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

}

#define DELAY 7
#include <util/delay_basic.h>

uint8_t spmap[]= {speaker1Pin,speaker2Pin,speaker3Pin,speaker1Pin};

//uint8_t sig[]= {1,2,1,2,1,2,1,2,0};
//uint8_t sig[]= {16,32,16,32,16,32,16,32,0};
//uint8_t sig[]= 
//{0,4,0,8,0,4,0,8,0,4,0,8,0,4,0,8,0,4,0,8,0,4,0,8,0,4,0,8,0,4,0,8,0};
uint8_t sigd[]= {8,8,8,8,8,8,8,8,8};

uint8_t sig[]= 
{4,8,4,8,4,8,4,8,
// 0,4,8,4,8,4,8,4,8,4,8,4,8,4,8,
4,8,4,8,4,8,4,8,4,8,4,8,4,8,4,8,4,8,
4,8,4,8,4,8,4,8,4,8,4,8,4,8,4,8,4,8,4,8,4,8,4,8,4,8,4,8,4,8,4,8,
4,8,4,8,4,8,4,8,4,8,4,8,4,8,4,8,4,8,4,8,4,8,4,8,4,8,4,8,4,8,4,8,
4,8,4,8,4,8,4,8,4,8,4,8,4,8,4,8,4,8,4,8,4,8,4,8,4,8,4,8,4,8,4,8,
0};


uint8_t dmax = 80-19+20;
uint8_t dmin = 80-19-20;//20;

uint8_t d = dmax;

ISR(PCINT0_vect) // same as above for PCINT1
{
  if(digitalRead(inPin1))
    return;
    
  byte saveSREG = SREG;
  noInterrupts();
      
    uint8_t i;
    volatile uint8_t j;  
    digitalWrite(ledPin, HIGH);
    for(i=0;i < sizeof(sig);i++) {
      PORTC = sig[i];
      
      j=d;
//      __builtin_avr_delay_cycles(d);
      _delay_loop_1(d);
//      while(--j);
//    delayMicroseconds(d);
    } 
    digitalWrite(ledPin, LOW);
    static uint8_t f=1;
    if(!--f) {
      d--;
      f=5;
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

