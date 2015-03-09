#include <CapacitiveSensor.h>

const int relay = A0;

long time;

CapacitiveSensor sensor =  CapacitiveSensor(2, 12);
long value;
boolean state = false;
boolean stateLamp = HIGH;

void setup() {
  Serial.begin(9600);
  Serial.println("Initialisation...");
  time = millis();
  pinMode(relay, OUTPUT);
}

void loop() {
  value = sensor.capacitiveSensor(30);
  if (value > 1500 && state == false) {
    state = true;
    stateLamp = !stateLamp;
    Serial.println("Switched !");
    digitalWrite(relay, stateLamp);
    time = millis();
    
  }
  if (value < 1200 && millis() - time > 500) {
    state = false;
  }
  Serial.println(value);
  delay(10);
}
