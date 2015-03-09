#include <Servo.h>

short potentio = A0;
int value;

Servo servo;

void setup() {
  // put your setup code here, to run once:
  servo.attach(11);
  Serial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:
  value = map(analogRead(potentio), 0, 1023, 0, 180);
  servo.write(value);
  Serial.println(value);
}
