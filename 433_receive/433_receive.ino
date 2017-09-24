#include <VirtualWire.h>

void setup() {
  Serial.begin(9600);
  vw_setup(2000);
  vw_rx_start();
}

void loop() {
  int valeurs[4];
  byte taille_message = sizeof(valeurs);
  vw_wait_rx();
  if (vw_get_message((byte *) &valeurs, &taille_message)) {
    Serial.print(valeurs[0]);
    Serial.print("\t");
    Serial.print(valeurs[1]);
    Serial.print("\t");
    Serial.print(valeurs[2]);
    Serial.print("\t");
    Serial.println(valeurs[3]);
  }
}
