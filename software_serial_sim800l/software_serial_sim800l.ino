// Connect VIO to +5V
// Connect GND to Ground
// Connect RX (data into SIM800L) to Digital 7
// Connect TX (data out from SIM800L) to Digital 8
// Importation librairie
#include <SoftwareSerial.h>
SoftwareSerial sim800l(7, 8); // RX, TX

void setup() {
Serial.begin(9600);
sim800l.begin(9600);
}

void loop() {
if (sim800l.available())
Serial.write(sim800l.read());

if (Serial.available()) {
while(Serial.available())   {
sim800l.write(Serial.read());
}
sim800l.println();
}
}
