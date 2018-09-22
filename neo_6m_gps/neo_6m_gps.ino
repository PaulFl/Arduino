#include <SoftwareSerial.h>

SoftwareSerial gpsReceiver(3,4);

void setup() {
  Serial.begin(9600);
  gpsReceiver.begin(9600);
}

void loop() {
  while(gpsReceiver.available() > 0){
    byte gpsData = gpsReceiver.read();
    //Serial.println(gpsData);
    Serial.write(gpsData);
  }
}
