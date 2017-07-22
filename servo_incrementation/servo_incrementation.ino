#include <Servo.h>

short potentio = A0;
int value;
int sPosition = 90;

Servo servo;

void setup() {
  // put your setup code here, to run once:
  servo.attach(11);
  Serial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:
  value = map(analogRead(potentio), 0, 1023, -10, 10);
  if (sPosition+value <= 180 && sPosition+value >= 0) {
    sPosition += value;
  }
  
  servo.write(sPosition);
  Serial.println(value);
  delay(50);
}
