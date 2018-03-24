#include <SoftwareSerial.h>

SoftwareSerial sSerial(10,11); //rx, tx

String serialIn = "";

void setup() {
  Serial.begin(9600);
  sSerial.begin(9600);
}

void loop() {
  while (sSerial.available()) {
    char inChar = (char)sSerial.read();
    serialIn+= inChar;
    if (inChar == '\n'){
      int id = serialIn.substring(0,1).toInt();
      Serial.println(id);
      bool state = serialIn.substring(1).toInt();
      Serial.println(state);
      serialIn = "";
    }
  }
}
