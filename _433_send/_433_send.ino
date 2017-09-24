#include <VirtualWire.h>

void setup() {
  Serial.begin(9600);

  // Initialisation de la bibliothèque VirtualWire
  // Vous pouvez changez les broches RX/TX/PTT avant vw_setup() si nécessaire
  vw_setup(2000);
  
  Serial.println("Go !"); 
}
 
void loop() {
  int valeurs[4];
  
  // Lit les broches analogiques
  valeurs[0] = 25;
  valeurs[1] = 100;
  valeurs[2] = 250;
  valeurs[3] = 180;
  
  vw_send((byte *) &valeurs, sizeof(valeurs)); // On envoie le message
  vw_wait_tx(); // On attend la fin de l'envoi
  
  delay(10);
}
