void setup() {
  Serial.begin(9600);
}

void loop() {
  Serial.print(analogRead(A0));
  Serial.print("  ");
  Serial.println(analogRead(A4));
  delay(10);
}
