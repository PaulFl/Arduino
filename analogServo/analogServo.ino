#include <Servo.h>

short analogInPin = A1;
short servoPin = 9;


Servo motor;

void setup() {
  motor.attach(servoPin);
}

void loop() {
  // Set motor
  int motorSpeed;
  motorSpeed = analogRead(analogInPin);
  motorSpeed = map(motorSpeed, 0, 1023, 20, 180);
  motor.write(motorSpeed);
  delay(10);
}
