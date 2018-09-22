#include <string.h>
#include <ctype.h>


int ledPin = 13;                  // LED test pin
int rxPin = 3;                    // RX PIN
int txPin = 4;                    // TX TX
int byteGPS = -1;
char linea[300] = "";
char comandoGPR[7] = "$GPRMC";
int cont = 0;
int bien = 0;
int conta = 0;
int indices[13];
void setup() {
  pinMode(ledPin, OUTPUT);       // Initialize LED pin
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
  Serial.begin(4800);
  for (int i = 0; i < 300; i++) { // Initialize a buffer for received data
    linea[i] = ' ';
  }
}
void loop() {
  digitalWrite(ledPin, HIGH);
  byteGPS = Serial.read();       // Read a byte of the serial port
  if (byteGPS == -1) {           // See if the port is empty yet
    delay(100);
  } else {
    // note: there is a potential buffer overflow here!
    linea[conta] = byteGPS;      // If there is serial port data, it is put in the buffer
    conta++;
    Serial.print(byteGPS, byte);
    if (byteGPS == 13) {         // If the received byte is = to 13, end of transmission
      // note: the actual end of transmission is <CR><LF> (i.e. 0x13 0x10)
      digitalWrite(ledPin, LOW);
      cont = 0;
      bien = 0;
      // The following for loop starts at 1, because this code is clowny and the first byte is the <LF> (0x10) from the previous transmission.
      for (int i = 1; i < 7; i++) { // Verifies if the received command starts with $GPR
        if (linea[i] == comandoGPR[i - 1]) {
          bien++;
        }
      }
      if (bien == 6) {           // If yes, continue and process the data
        for (int i = 0; i < 300; i++) {
          if (linea[i] == ',') { // check for the position of the  "," separator
            // note: again, there is a potential buffer overflow here!
            indices[cont] = i;
            cont++;
          }
          if (linea[i] == '*') { // ... and the "*"
            indices[12] = i;
            cont++;
          }
        }
        Serial.println("");      // ... and write to the serial port
        Serial.println("");
        Serial.println("---------------");
        for (int i = 0; i < 12; i++) {
          switch (i) {
            case 0 : Serial.print("Time in UTC (HhMmSs): "); break;
            case 1 : Serial.print("Status (A=OK,V=KO): "); break;
            case 2 : Serial.print("Latitude: "); break;
            case 3 : Serial.print("Direction (N/S): "); break;
            case 4 : Serial.print("Longitude: "); break;
            case 5 : Serial.print("Direction (E/W): "); break;
            case 6 : Serial.print("Velocity in knots: "); break;
            case 7 : Serial.print("Heading in degrees: "); break;
            case 8 : Serial.print("Date UTC (DdMmAa): "); break;
            case 9 : Serial.print("Magnetic degrees: "); break;
            case 10 : Serial.print("(E/W): "); break;
            case 11 : Serial.print("Mode: "); break;
            case 12 : Serial.print("Checksum: "); break;
          }
          for (int j = indices[i]; j < (indices[i + 1] - 1); j++) {
            Serial.print(linea[j + 1]);
          }
          Serial.println("");
        }
        Serial.println("---------------");
      }
      conta = 0;                  // Reset the buffer
      for (int i = 0; i < 300; i++) { //
        linea[i] = ' ';
      }
    }
  }
}
