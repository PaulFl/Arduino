#include <Servo.h>

short analogInPin = A0;
short servoPin = 9;
short lipoCellPin = A1;

int analogMax = 725;
float voltageRef = 4.63;

float voltage;
int motorSpeed;

Servo motor;

void setup() {
  motor.attach(servoPin);
  Serial.begin(9600);
}

void loop() {
  // Set motor
  motorSpeed = analogRead(analogInPin);
  motorSpeed = map(motorSpeed, 0, analogMax, 20, 180);
  Serial.print("Motor: ");
  Serial.println(motorSpeed);
  motor.write(motorSpeed);

  //Battery percentage
  float voltage = analogRead(lipoCellPin) * (voltageRef/1023.0);
  Serial.print("Battery voltage: ");
  Serial.println(voltage);
  delay(100);
}
