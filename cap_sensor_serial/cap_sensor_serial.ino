#include <CapacitiveSensor.h>

CapacitiveSensor capSensor = CapacitiveSensor(A5, A2);

void setup() {
  Serial.begin(9600);
}

void loop() {
  Serial.println(capSensor.capacitiveSensorRaw(50));
  delay(25);
}
