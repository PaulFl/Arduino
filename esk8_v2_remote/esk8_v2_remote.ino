#include <nRF24L01.h>
#include <printf.h>
#include <RF24.h>
#include <RF24_config.h>

#define ANALOGMINVERTICAL 0
#define ANALOGMAXVERTICAL 1023
#define ANALOGMINHORIZONTAL 0
#define ANALOGMAXHORIZONTAL 1023

#define VERTICALMIN 0
#define VERTICALMAX 180
#define HORIZONTALMIN 0
#define HORIZONTALMAX 100

#define DELAY 25

short cButtonPin = A5;
short zButtonPin = A4;

short analogVerticalPin = A0;
short analogHorizontalPin = A1;

short ledPin = 6;

short RF24cePin = 7;
short RF24csnPin = 8;

int msg[4];
RF24 radio(RF24cePin, RF24csnPin);
const uint64_t pipe = 0xE8E8F0F0E1LL;

void setup() {
  radio.begin();
  radio.openWritingPipe(pipe);

  pinMode(ledPin, OUTPUT);
  pinMode(cButtonPin, INPUT_PULLUP);
  pinMode(zButtonPin, INPUT_PULLUP);
  pinMode(analogVerticalPin, INPUT);
  pinMode(analogHorizontalPin, INPUT);
  
  digitalWrite(ledPin, HIGH);
}

void loop() {
  delay(DELAY);
  
  msg[0] = map(analogRead(analogVerticalPin), ANALOGMINVERTICAL, ANALOGMAXVERTICAL, VERTICALMIN, VERTICALMAX);
  msg[1] = map(analogRead(analogHorizontalPin), ANALOGMINHORIZONTAL, ANALOGMAXHORIZONTAL, HORIZONTALMIN, HORIZONTALMAX);
  msg[2] = int(digitalRead(cButtonPin));
  msg[3] = int(digitalRead(zButtonPin));
  
  radio.write(msg,4);
}
