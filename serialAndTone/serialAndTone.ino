#include <SoftwareSerial.h>
#include <toneAC.h>

SoftwareSerial serial(10, 10);

void setup() {
  Serial.begin(9600);
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  serial.begin(9600);
  while(!serial.available()){
  }
  while(serial.available()){
    Serial.print(serial.read());
  }
  serial.end();
  toneAC(3000, 5, 500, true);
  delay(600);
  toneAC(4000, 8, 500, true);
  delay(600);
  while(serial.available()){
    Serial.print(serial.read());
  }
  digitalWrite(13, HIGH);
  while(!serial.available()){
  }
  toneAC(3000, 5, 500, true);
  delay(600);
  toneAC(4000, 8, 500, true);
  delay(600);

}

void loop() {
  // put your main code here, to run repeatedly:

}
