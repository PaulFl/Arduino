
#include "FlySkyIBus.h"
#include <SoftwareSerial.h>

SoftwareSerial mySerial(10, 11); // RX, TX


void setup() 
{
  Serial.begin(9600);
  IBus.begin(mySerial);
}

void loop() 
{
  IBus.loop();
  Serial.println(IBus.readChannel(2), HEX);
}
