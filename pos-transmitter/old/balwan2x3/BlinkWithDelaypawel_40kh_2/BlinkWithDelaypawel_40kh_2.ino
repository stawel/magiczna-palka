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

//uint8_t sig[]= {1,2,1,2,1,2,1,2,0};
//uint8_t sig[]= {16,32,16,32,16,32,16,32,0};

#define C 50
#define D -10-20
#define E 6-20

#define A (2)
#define B (50)

#define E (+2)

uint8_t sig1_t[]= {
61-E,61-E,
//60,60,
61-E,
//60,60,
61-E,61-E,

100,

50,50,
//60,60,
50,
//60,60,
50,50,

};

#define C -5
#define D +5

uint8_t sig_t[]= {
    65-(C),65-(C),65-(C),65-(C),
    50-(D),50-(D),50-(D),50-(D),
};

uint8_t sig1[]= {
//    60-(C),60-(C),60-(C),65-(C),65-(C),65-(C),
//    55-(C),55-(C),55-(C),55-(C),
 1,2, 0, 1,2, 0,
// 1,2, 0, 1,2, 

//30+60,60,  60,60, 60,60,
//60,60, 60,60, 60,60, 60,60,

/*
0*A+B,
1*A+B,
2*A+B,
3*A+B,
4*A+B,
5*A+B,
6*A+B,
1*A+B,
1*A+B,
1*A+B,
*/


//    60-(E),60-(E),60-(E),60-(E),60-(E)

  /*    1,2,1,2,1,2,1,2,
    1,2,1,2,1,2,1,2,
    1,2,1,2,1,2,1,2,1,2,1,2,1,2,1,2,
    */
//    0
//  0,1,0,2,0,1,0,2,0,1,0,2,0,1,0,2,
//  0,1,0,2,0,1,0,2,0,1,0,2,0,1,0,2,

/*  0,0,
  0,1,0,2,0,1,0,2,0,1,0,2,0,1,0,2,
  0,1,0,2,0,1,0,2,0,1,0,2,0,1,0,2,
  0,1,0,2,0,1,0,2,0,1,0,2,0,1,0,2,
  0,};*/
  };
uint8_t sig2[]= {
  0,4,0,8,0,4,0,8,0,4,0,8,0,4,0,8,
  0,4,0,8,0,4,0,8,0,4,0,8,0,4,0,8,
  0,0,
//}; 
  0,4,0,8,0,4,0,8,0,4,0,8,0,4,0,8,
  0,4,0,8,0,4,0,8,0,4,0,8,0,4,0,8,
  0,4,0,8,0,4,0,8,0,4,0,8,0,4,0,8,
  0,};
uint8_t sig3[]= {
  0,16,0,32,0,16,0,32,0,16,0,32,0,16,0,32,
  0,16,0,32,0,16,0,32,0,16,0,32,0,16,0,32,
  0,0,
//};
  0,16,0,32,0,16,0,32,0,16,0,32,0,16,0,32,
  0,16,0,32,0,16,0,32,0,16,0,32,0,16,0,32,
  0,16,0,32,0,16,0,32,0,16,0,32,0,16,0,32,
  0,};


//uint8_t dmax = 29;//29
//uint8_t dmin = 29;//30;//30-1;//29

uint8_t dmax = 65+35;//60-20;//29
uint8_t dmin = 65-35;//60-5;//30;//30-1;//29

//uint8_t dmax = 29+20;
//uint8_t dmin = 29;

uint8_t d = dmax;


void speaker123(uint8_t *sig)
{
    uint8_t i;
    for(i=0;i<sizeof(sig1);i++) {
      PORTC = sig[i];
      _delay_loop_1(d);
    } 
//    PORTC = 0;      
}

void speaker4(uint8_t *sig)
{
    uint8_t i;//,d=60;
    uint8_t d2 = 65;
    uint8_t d = 50;
//    for(i=0;i<sizeof(sig_t);i++) {
//      d=sig_t[i];
//      PORTD = 1;      
//      _delay_loop_1(d);
//      PORTD = 2;      
//      _delay_loop_1(d);
//    }

      PORTD = 1;      _delay_loop_1(d2);
      PORTD = 2;      _delay_loop_1(d2);
//      PORTD = 1;      _delay_loop_1(d2);
//      PORTD = 2;      _delay_loop_1(d2);
      PORTD = 0;      _delay_loop_1(d2);
//      PORTD = 1;      _delay_loop_1(d2);
//      PORTD = 2;      _delay_loop_1(d2);
      PORTD = 1;      _delay_loop_1(d2);
      PORTD = 2;      _delay_loop_1(d2);
      PORTD = 1;      _delay_loop_1(d2);
      PORTD = 2;      _delay_loop_1(d2);
     PORTD = 0;      
}


ISR(PCINT0_vect) // same as above for PCINT1
{
  if(digitalRead(inPin1))
    return;
    
  byte saveSREG = SREG;
  noInterrupts();
      
    volatile uint8_t j;
    uint8_t *sig = sig1; 
    uint8_t witch = (1&digitalRead(inPin2)) + ((1&digitalRead(inPin3)) << 1);
    switch(witch) {
      case 0: sig=sig1; break;
      case 1: sig=sig2; break;
      case 2: sig=sig3; break;
      case 3: sig=sig1; break;
    }
    
    digitalWrite(ledPin, HIGH);
//    if(witch == 3)
    if(true)
      speaker4(sig1);
    else
      speaker123(sig);
    
    digitalWrite(ledPin, LOW);
    static uint8_t f=35;
    if(!--f) {
      d--;
      f=35;
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

