#include "segments.h"
#include "pins.h"

#define DEBUG 0
#define REFRESHSPEED 2

unsigned long flipmillis;

boolean segments[6][8];
short onSegment = 0;

void setup() {
	if (DEBUG) {
		Serial.begin(9600);
	}
	for (int i = 0; i < 6; i++) {
		pinMode(cathodeSegments[i], OUTPUT);
		digitalWrite(cathodeSegments[i], HIGH);
	}
	for (int i = 0; i < 8; i++) {
		pinMode(anodeSegments[i], OUTPUT);
		digitalWrite(anodeSegments[i], LOW);
	}

}

void loop() {

}

void display(short text[6]) {
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 7; j++) {
			segments[i][j] = chars[text[i]][j];
		}
	}
	
}

void setSeparators(boolean separators[6]) {
	for (int i = 0; i < 6; i++) {
		segments[i][7] = separators[i];
	}
}

void flip() {
	if (flipmillis - millis() > REFRESHSPEED) {
		digitalWrite(onSegment, HIGH);
		onSegment++;
		if (onSegment == 6) {
			onSegment = 0;
		}
		digitalWrite(onSegment, LOW);
		for (int i = 0; i < 8; i++) {
			digitalWrite(anodeSegments[i], segments[onSegment][i]);
		}
		flipmillis = millis();
	}
}
