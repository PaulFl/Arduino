void setup() {
  Serial.begin(9600);
  pinMode(1, OUTPUT);
}

void loop() {
  digitalWrite(1, HIGH);
  delay(50);
  digitalWrite(1, LOW);
  delay(300);
  if (Serial.available()) {
    digitalWrite(1, HIGH);
    delay(1000);
  }
}
