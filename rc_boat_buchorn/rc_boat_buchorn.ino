int value;
void setup() {
pinMode(3, OUTPUT);
digitalWrite(3, LOW);
pinMode(8, INPUT);
digitalWrite(3,HIGH);

}

void loop() {
  delay(50);
  value = pulseIn(8, HIGH);
  if (value < 1000) {
    digitalWrite(3, HIGH);
  }else{
    digitalWrite(3, LOW);
  }
}
