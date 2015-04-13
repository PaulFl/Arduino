#include <CapacitiveSensor.h>

short sendPin = A5;
short receivePin = A4;

unsigned long pastMicros;

CapacitiveSensor sensor = CapacitiveSensor(sendPin, receivePin);

void setup() {
	Serial.begin(9600);
}

void loop() {
	pastMicros = micros();
	Serial.print(sensor.capacitiveSensor(10));
	Serial.print("\t");
	Serial.println(micros() - pastMicros);
	delay(250);
}
