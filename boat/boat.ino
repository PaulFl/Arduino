int value;
void setup() {
pinMode(2, OUTPUT);
digitalWrite(2, LOW);
pinMode(8, INPUT);

}

void loop() {
  delay(50);
  value = pulseIn(8, HIGH);
  if (value < 1000) {
    digitalWrite(2, HIGH);
  }else{
    digitalWrite(2, LOW);
  }
}
