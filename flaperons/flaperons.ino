#include <Servo.h>

#define FLAPSPERCENTAGE 40

//Pins
short leftPin = 3;
short rightPin = 4;
short flapsPin = 5;
short aileronsPin = 6;

float expo = 1.0;
unsigned int pulseMin = 0;
unsigned int pulseMax = 1000;
unsigned int servoMin = 0;
unsigned int servoMax = 180;
unsigned int flapsMin = 0;
unsigned int flapsMax = servoMax * FLAPSPERCENTAGE / 100;

Servo left;
Servo right;

unsigned int ailerons;
unsigned int flaps;

unsigned int leftAileron;
unsigned int rightAileron;

void setup() {
  left.attach(leftPin);
  right.attach(rightPin);

  pinMode(flapsPin, INPUT);
  pinMode(aileronsPin, INPUT);
}

void loop() {
  ailerons = pulseIn(aileronsPin, HIGH);
  flaps = pulseIn(flapsPin, HIGH);
  ailerons = map(ailerons, pulseMin, pulseMax, servoMin, servoMax);
  flaps = map(flaps, pulseMin, pulseMax, flapsMin, flapsMax);
  leftAileron = flaps + ailerons;
  rightAileron = flaps - ailerons;
  left.write(leftAileron);
  right.write(rightAileron);
}
