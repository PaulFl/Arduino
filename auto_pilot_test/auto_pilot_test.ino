#include <Servo.h>

short servo_1_select = A0;
short servo_2_select = A1;

short servo_1_ap = 6;
short servo_2_ap = 9;

Servo servo_1;
Servo servo_2;

void setup() {
	pinMode(servo_1_select, OUTPUT);
	pinMode(servo_2_select, OUTPUT);

	digitalWrite(servo_1_select, HIGH);
	digitalWrite(servo_2_select, HIGH);

	servo_1.attach(servo_1_ap);
	servo_2.attach(servo_2_ap);


}

void loop() {
	servo_2.write(40);
	servo_1.write(90);
	delay(2000);
	servo_2.write(120);
	servo_1.write(100);
	delay(2000);
}
