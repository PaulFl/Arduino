#include <nRF24L01.h>
#include <printf.h>
#include <RF24.h>
#include <RF24_config.h>

#define ANALOGMIN 0
#define ANALOGMAX 1023
#define MOTORMIN 30
#define MOTORMAX 180
#define DELAY 25

short brakeButtonPin = A5;
short motorButtonPin = A4;
short analogInPin = A0;
short ledPin = 6;

short RF24cePin = 7;
short RF24csnPin = 8;

int msg[1];
RF24 radio(RF24cePin, RF24csnPin);
const uint64_t pipe = 0xE8E8F0F0E1LL;

void setup(void) {
  radio.begin();
  radio.openWritingPipe(pipe);
  pinMode(ledPin, OUTPUT);
  pinMode(brakeButtonPin, INPUT_PULLUP);
  pinMode(motorButtonPin, INPUT_PULLUP);
  pinMode(analogInPin, INPUT);
  digitalWrite(ledPin, HIGH);
}

void loop(void) {
  delay(DELAY);
  if (digitalRead(motorButtonPin) == LOW) {
    msg[0] = map(analogRead(analogInPin), ANALOGMIN, ANALOGMAX, MOTORMIN, MOTORMAX);
  } else if (digitalRead(brakeButtonPin) == LOW) {
    msg[0] = 200;
  } else {
    msg[0] = 0;
  }
  radio.write(msg, 1);
}
