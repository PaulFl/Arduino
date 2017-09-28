#include <VirtualWire.h>

void setup() {
  Serial.begin(9600);
  vw_setup(2000);
}

void loop() {
  int valeurs[4];

  valeurs[0] = 25;
  valeurs[1] = 100;
  valeurs[2] = 250;
  valeurs[3] = 180;

  vw_send((byte *) &valeurs, sizeof(valeurs));
  vw_wait_tx();

  delay(10);
}
