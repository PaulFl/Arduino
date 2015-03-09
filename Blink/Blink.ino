// Blink.ino
int pLed = A0;

void setup() {
	pinMode(pLed, OUTPUT);
}

void loop() {
	digitalWrite(pLed, HIGH);
	delay(1000);
	digitalWrite(pLed, LOW);
	delay(1000);
}

