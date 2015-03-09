// RCServo.ino
#include <Servo.h>

Servo servo;

int pos;
int speedServo = 10;
String inString = " ";

void setup() {
	Serial.begin(9600);
	Serial.println("Initialisation...");
	servo.attach(9);
}

void loop() {
	if(pos >= 0 && pos <= 180){
	    servo.write(pos);
	    delay(speedServo);
	}
}

void serialEvent() {
	int inChar;
	while(Serial.available() > 0){
	    inChar = Serial.read();
	    if(isDigit(inChar)){
	        inString += (char)inChar;
	    }
	    if(inChar == '\n'){
	        Serial.print("Value: ");
	        pos = inString.toInt();
	        Serial.println(pos);
	        inString = " ";
	    }
	}
}
