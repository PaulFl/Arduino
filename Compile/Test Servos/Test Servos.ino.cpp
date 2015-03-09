#include <Arduino.h>
// Servos.ino

#include <Servo.h>

Servo leftRight;
Servo profondeur;

int pos;
int maximum = 140;



void setup() {
	leftRight.attach(9);
	profondeur.attach(10);
}

void loop() {
	for(pos=0; pos<= maximum; pos++){
	    leftRight.write(pos);
	    profondeur.write(pos);
	    delay(15);
	}
	for(pos=maximum; pos>= 0; pos--){
	    leftRight.write(pos);
	    profondeur.write(pos);
	    delay(15);
	}
}