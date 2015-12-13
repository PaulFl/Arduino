#define DEBUG 0
#define HIGHTHRESHOLD 250
#define LOWTHRESHOLD 50

#include <CapacitiveSensor.h>

short sendPin = A5;
short receivePin = A2;

short relay = 9;
short led = 13;

bool relayState = LOW;
bool touched = false;

CapacitiveSensor capSensor = CapacitiveSensor(sendPin, receivePin);
long sensorValue;

void setup() {
  if (DEBUG) Serial.begin(9600);

  pinMode(relay, OUTPUT);
  digitalWrite(relay, LOW);

  pinMode(led, OUTPUT);
  pinMode(led, HIGH);
}

void loop() {
  sensorValue = capSensor.capacitiveSensor(30);

  if (DEBUG) Serial.println(sensorValue);

  if (sensorValue > HIGHTHRESHOLD && !touched) {
    touched = true;
    digitalWrite(led, HIGH);
    relayState = !relayState;
    if (DEBUG) Serial.println("Switched");
    digitalWrite(relay, relayState);
  } else if (sensorValue < LOWTHRESHOLD && touched) {
    touched = false;
    digitalWrite(led, LOW);
  }
  delay(10);
}



