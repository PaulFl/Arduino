#include <SoftwareSerial.h>
#include <toneAC.h>

SoftwareSerial serial(10, 10);

void setup() {
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  serial.begin(9600);
  while(!serial.available()){
  }
  serial.end();
  toneAC(3000, 5, 500, true);
  delay(600);
  toneAC(4000, 8, 500, true);
  delay(600);
  serial.begin(9600);
  while(!serial.available()){
  }
  digitalWrite(13, HIGH);

}

void loop() {
  // put your main code here, to run repeatedly:

}
