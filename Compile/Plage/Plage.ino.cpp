#include <Arduino.h>
// Plage.ino

#include <String.h>

#define ROUGE 13
#define ORANGE 12
#define VERT 11

const int button = 2;
boolean alarm = false;
int temps;
char input[20] = {0};
int etat = VERT;
boolean stateAlarmLed;


void serialEvent();
void leds(int led);
void buttonPressed ();

void setup() {
	Serial.begin(9600);
	Serial.println("Initialisation...");
	pinMode(VERT, OUTPUT);
	pinMode(ORANGE, OUTPUT);
	pinMode(ROUGE, OUTPUT);
	pinMode(button, INPUT_PULLUP);
	leds(etat);
	attachInterrupt(button-2, buttonPressed, FALLING);
	temps = millis();
}

void loop() {
	if (alarm){
		if(millis()-temps > 500){
		    temps = millis();
		    stateAlarmLed = !stateAlarmLed;
		    digitalWrite(ROUGE, stateAlarmLed);
		}
	}
	else {
		Serial.println(input);
	if(strcmp(input, "tempete") == 0){
	    etat = ROUGE;
	    Serial.println("oui");
	    leds(etat);
	}
	else if(strcmp(input, "vagues") == 0){
	    etat = ORANGE;
	    leds(etat);
	}
	else if(strcmp(input, "calme") == 0){
		etat = VERT;
		leds(etat);
	}
}
}

void serialEvent() {
	char inChar;
	int i = 0;
	while(Serial.available()>0){
	    inChar = Serial.read();
	    if (inChar != '\n') {
	    	input[i] = inChar;
	    }
	    else {
	    	input[i] = '\0';
	    	break;
	    }
	    i++;
	}
	Serial.print(input);
}
void leds(int led) {
	digitalWrite(VERT, LOW);
	digitalWrite(ORANGE, LOW);
	digitalWrite(ROUGE, LOW);
	digitalWrite(led, HIGH);
}

void buttonPressed () {
	alarm = !alarm;
	stateAlarmLed = LOW;
	leds(etat);
}