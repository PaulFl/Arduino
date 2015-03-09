// Light.ino

int const red = 13; // Anode sur pin 13 et cathode sur GND
int const green = 12;
int const button = 2; // Sur pin 2 et GND
volatile int lightOn = LOW;

void buttonPressed () {
	lightOn = !lightOn;
	Serial.println("-");
}

void setup() {
	pinMode(red, OUTPUT);
	pinMode(green, OUTPUT);
	pinMode(button, INPUT_PULLUP);
	attachInterrupt(button-2, buttonPressed, FALLING);
	Serial.begin(9600);
}

void loop() {
	digitalWrite(red, lightOn);
	digitalWrite(green, !lightOn);
}

