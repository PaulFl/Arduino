short ledPin = 13;

void setup() {
	pinMode(ledPin, OUTPUT);
	Serial.begin(9600);
}

void loop() {
	Serial.println(Serial.available());
	digitalWrite(ledPin, HIGH);
	delay(250);
	digitalWrite(ledPin, LOW);
	delay(250);
	Serial.println("Done");
}
