#include <Servo.h>

short pot_1_pin = A0;
short pot_2_pin = A1;
short pot_3_pin = A2;

short servo_1_pin = 9;
short servo_2_pin = 10;
short servo_3_pin = 11;

int value_1;
int value_2;
int value_3;

Servo servo_1;
Servo servo_2;
Servo servo_3;

void setup() {
  servo_1.attach(servo_1_pin);
  servo_2.attach(servo_2_pin);
  servo_3.attach(servo_3_pin);
}

void loop() {
  value_1 = analogRead(pot_1_pin);
  value_2 = analogRead(pot_2_pin);
  value_3 = analogRead(pot_3_pin);

  value_1 = map(value_1, 0, 1023, 0, 180);
  value_2 = map(value_2, 0, 1023, 0, 180);
  value_3 = map(value_3, 0, 1023, 0, 180);

  servo_1.write(value_1);
  servo_2.write(value_2);
  servo_3.write(value_3);

  delay(15);
}
