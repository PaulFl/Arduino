/*
 * Test FlySky IBus interface on an Arduino Mega.
 *  Connect FS-iA6B receiver to Serial1.
 */
 
#include "FlySkyIBus.h"

void setup() 
{
  IBus.begin(Serial);
  pinMode(13, OUTPUT);
}

void loop() 
{
  IBus.loop();
  digitalWrite(13, IBus.readChannel(0) > 1500);
}
