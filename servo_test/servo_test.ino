#include <Servo.h>

Servo servo;

void setup(){
servo.attach(9);
}

void loop(){
servo.write(45);
delay(1000);
servo.write(143);
delay(1000);
}
