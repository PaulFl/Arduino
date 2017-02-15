//Conductive surface (capacitive sensor) used to switch a relay
//Adjust onThreshold and offT5hreshold values to suit the capacitive sensor surface (I an using a can here)


#include <CapacitiveSensor.h>

#define DEBUG 1

int onThreshold = 3100;
int offThreshold = 2200;

short relay = 9;
CapacitiveSensor sensor =  CapacitiveSensor(A5, A2);

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
  value = sensor.capacitiveSensorRaw(100);
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
  delay(30);
}
