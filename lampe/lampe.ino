#include <CapacitiveSensor.h>

#define DEBUG 1

int onThreshold = 1500;
int offThreshold = 1200;

short relay = 10;
CapacitiveSensor sensor =  CapacitiveSensor(2, 12);

long time;
long value;

boolean state = false;
boolean stateLamp = LOW;

void setup() {
  if (DEBUG) {
    Serial.begin(9600);
    Serial.println("Initialisation...");
  }
  time = millis();
  pinMode(relay, OUTPUT);
  digitalWrite(relay, stateLamp);
}

void loop() {
  value = sensor.capacitiveSensor(30);
  if (value > onThreshold && state == false) {
    state = true;
    stateLamp = !stateLamp;
    if (DEBUG) Serial.println("Switched !");
    digitalWrite(relay, stateLamp);
  } else if (value < offThreshold && state == true) {
    state = false;
  }
  if (DEBUG) {
    Serial.println(value);
    delay(10);
  }
}
