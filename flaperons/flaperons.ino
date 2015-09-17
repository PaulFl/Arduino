#include <Servo.h>

#define FLAPSPERCENTAGE 40
#define LISSAGE 5

//Pins
short leftPin = 9;
short rightPin = 10;
short flapsPin = 6;
short aileronsPin = 5;

float expo = 1.0;
int aileronsPulseMin = 1490;
int aileronsPulseMax = 1920;
int flapsPulseMin = 810;
int flapsPulseMax = 1730;
int servoMin = 0;
int servoMax = 180;
int flapsMin = 0;
int flapsMax = servoMax * FLAPSPERCENTAGE / 100;

int lastValues[LISSAGE][2];
short lissageArrayIndex = 0;

Servo left;
Servo right;

int ailerons;
int flaps;

int leftAileron;
int rightAileron;

void setup() {
  Serial.begin(9600);
  left.attach(leftPin);
  right.attach(rightPin);

  pinMode(flapsPin, INPUT);
  pinMode(aileronsPin, INPUT);
}

void loop() {
  ailerons = pulseIn(aileronsPin, HIGH);
  lastValues[lissageArrayIndex][0] = ailerons;
  flaps = pulseIn(flapsPin, HIGH);
  lastValues[lissageArrayIndex][1] = flaps;
  lissageArrayIndex ++;
  if (lissageArrayIndex == LISSAGE){
    lissageArrayIndex = 0;
  }
  ailerons = 0;
  flaps = 0;
  for (int i = 0; i<LISSAGE; i++){
    ailerons += lastValues[i][0];
    flaps += lastValues[i][1];
   }
   ailerons = ailerons / LISSAGE;
   flaps = flaps / LISSAGE;
  ailerons = map(ailerons, aileronsPulseMin, aileronsPulseMax, -90, 90);
  flaps = map(flaps, flapsPulseMin, flapsPulseMax, flapsMin, flapsMax);
  leftAileron = flaps + ailerons;
  rightAileron = flaps - ailerons;
  Serial.println(leftAileron);
  left.write(leftAileron);
  right.write(rightAileron);
}
