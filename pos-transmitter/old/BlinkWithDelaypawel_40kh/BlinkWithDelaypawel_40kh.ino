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

// constants won't change. Used here to 
// set pin numbers:
const int ledPin =  13;      // the number of the LED pin
const int inPin1 =  9;      // the number of the LED pin
const int inPin2 =  10;      // the number of the LED pin
const int inPin3 =  11;      // the number of the LED pin
const int inLowPin =  12;      // the number of the LED pin
const int speaker1Pin =  8;      // the number of the LED pin
const int speaker1LowPin =  7;      // the number of the LED pin
const int speaker2Pin =  6;      // the number of the LED pin
const int speaker2LowPin =  5;      // the number of the LED pin
const int speaker3Pin =  4;      // the number of the LED pin
const int speaker3LowPin =  3;      // the number of the LED pin

// Variables will change:
int ledState = LOW;             // ledState used to set the LED
long previousMillis = 0;        // will store last time LED was updated

// the follow variables is a long because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long interval = 1000;           // interval at which to blink (milliseconds)

void setup() {
  // set the digital pin as output:

  PCICR |= (1 << PCIE0); // enable PCINT0
  PCMSK0 |= (1 << PCINT1); 
  
 // attachInterrupt(4/*inPin*/, blink, LOW);
  interrupts(); 

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


uint8_t spmap[]= {speaker1Pin,speaker2Pin,speaker3Pin,speaker1Pin};
ISR(PCINT0_vect) // same as above for PCINT1
{
  byte saveSREG = SREG;
  noInterrupts();
    uint8_t ile =2;
    uint8_t ile2 =5;
    
    uint8_t speaker;
    uint8_t witch = (1&digitalRead(inPin2)) + ((1&digitalRead(inPin3)) << 1);
    speaker = spmap[witch];
    
    digitalWrite(ledPin, HIGH);
    while(ile--) {
      digitalWrite(speaker, HIGH);
      delayMicroseconds(9);
      digitalWrite(speaker, LOW);
      delayMicroseconds(9);
    }
      digitalWrite(speaker, HIGH);
      delayMicroseconds(9);
    while(ile2--) {
      digitalWrite(speaker, HIGH);
      delayMicroseconds(9);
      digitalWrite(speaker, LOW);
      delayMicroseconds(9);
    }
    digitalWrite(ledPin, LOW);
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

