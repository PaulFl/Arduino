#include <SoftwareSerial.h>

SoftwareSerial serial(0, 0);

void setup() {
  pinMode(1, OUTPUT);
  serial.begin(9600);
}

void loop() {
  digitalWrite(1, LOW);
  if(serial.available()){
    serial.read();
    digitalWrite(1, HIGH);
    delay(1000);
  }
}
