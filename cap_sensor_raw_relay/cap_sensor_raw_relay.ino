#include <CapacitiveSensor.h>

bool touched = false;
bool stateRelay = false;

long value;

CapacitiveSensor capSensor = CapacitiveSensor(A5, A2);

void setup() {
  Serial.begin(9600);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(13, OUTPUT);
}

void loop() {
  value = capSensor.capacitiveSensorRaw(100);
  Serial.println(value);
  if (value > 3700 && !touched) {
    touched = true;
    digitalWrite(13, HIGH);
    stateRelay = !stateRelay;
    digitalWrite(9, stateRelay);
    digitalWrite(10, stateRelay);
  } else if (value < 3500 && touched) {
    touched = false;
    digitalWrite(13, LOW);
  }
  delay(5);
}
