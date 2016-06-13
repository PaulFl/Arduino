#include <Servo.h>

short switch = 5;
short doorServo = 9;
short armServo = 10;

Servo door;
Servo arm;

void setup() {
  door.attach(doorServo);
  arm.attach(armServo);

}

void loop() {

}


