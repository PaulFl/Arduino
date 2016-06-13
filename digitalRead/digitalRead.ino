void setup() {
  Serial.begin(9600);
  pinMode(A0, INPUT_PULLUP);
}

void loop() {
  delay(10);
  Serial.println(digitalRead(A0));
}
