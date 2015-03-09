int value;

void setup() {
  Serial.begin(9600);
  Serial.println("Initialisation");
  for (int i = 11; i<=13; i++) {
    pinMode(i, OUTPUT);
  }
}

void loop() {
  delay(100);
  value = map(analogRead(0), 0, 1023, 3, 0);
  Serial.println(value);
  if (value >= 1) {
    digitalWrite(11, HIGH);
  }
  else {
    digitalWrite(11, LOW);
  }
  if (value >= 2) {
    digitalWrite(12, HIGH);
  }
  else {
    digitalWrite(12, LOW);
  }
  if (value >= 3) {
    digitalWrite(13, HIGH);
  }
  else {
    digitalWrite(13, LOW);
  }
}
