// Connect VIO to +5V
// Connect GND to Ground
// Connect RX (data into SIM800L) to Digital 7
// Connect TX (data out from SIM800L) to Digital 8
// Importation librairie
#include <SoftwareSerial.h>
SoftwareSerial sim800l(7, 8); // RX, TX
String phoneNumber = "+33651239763";

void setup() {
  Serial.begin(9600);
  sim800l.begin(9600);
  Serial.println("Initialisation");
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  delay(300);
  digitalWrite(13, HIGH);
  delay(500);
  digitalWrite(13, LOW);
  sendAT("AT"); //Init baud
  String operateur = sendAT("AT+COPS ?"); //Opérateur
  operateur.remove(0, 24);
  operateur.remove(8);
  Serial.println(operateur);
  String signal = sendAT("AT+CSQ"); //Signal
  signal.remove(19);
  signal.remove(0, 15);
  Serial.println(signal);
  sendAT("AT+CMGF=1");
  sendAT("AT+CMGS=\"" + phoneNumber + "\"");
  sendAT("Chauffage boot");
  sendAT("Logs:");
  sendAT("Operateur: " + operateur);
  sendAT("Signal: " + signal);
  sim800l.write(26);
  delay(10000);
  sendAT("AT+CMGD=1,4"); //delete all messages

}

void loop() {
  delay(100);
  
  String messages = sendAT("AT+CMGL=\"REC UNREAD\"");
  if (messages.length() != 27) {
    String message = sendAT("AT+CMGR=1");
    Serial.println(message);
    sendAT("AT+CMGD=1,4"); //delete all messages

  }
}

String sendAT (String string) {
  sim800l.println(string);
  delay(100);
  String inString = "";
  if (sim800l.available()) {
    while (sim800l.available()) {
      char inChar = sim800l.read();
      inString += inChar;
    }
  }
  return inString;
}

