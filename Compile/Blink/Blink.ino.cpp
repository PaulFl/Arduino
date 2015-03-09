#include <Arduino.h>
// Blink.ino
int pLed = 13;



void setup() {
	pinMode(pLed, OUTPUT);
}

void loop() {
	digitalWrite(pLed, HIGH);
	delay(1000);
	digitalWrite(pLed, LOW);
	delay(75);
}

