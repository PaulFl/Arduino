float voltageRef = 4.65;
short lipoCellPin = A1;

void setup() {
Serial.begin(9600);
}

void loop() {
  Serial.println(analogRead(lipoCellPin) * (voltageRef/1023.0));
  delay(100);
}
