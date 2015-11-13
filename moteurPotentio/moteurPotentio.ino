short pin1Motor = 3;
short pin2Motor = 5;
short pinSpeedMotor = 9;

short analogInput = A5;

void setup() {
  pinMode(pin1Motor, OUTPUT);
  pinMode(pin2Motor, OUTPUT);
  pinMode(pinSpeedMotor, OUTPUT);
  pinMode(analogInput, INPUT);

  digitalWrite(pin1Motor, LOW);
  digitalWrite(pin2Motor, LOW);
  digitalWrite(pinSpeedMotor, LOW);
}

void loop() {
  int value = analogRead(analogInput);
  value = map(value, 0, 1023, -1000, 1000);
  if (value < -50) {
    digitalWrite(pin1Motor, HIGH);
    digitalWrite(pin2Motor, LOW);
    analogWrite(pinSpeedMotor, map(value, -1000, -50, 255, 0));
  } else if (value > 50) {
    digitalWrite(pin1Motor, LOW);
    digitalWrite(pin2Motor, HIGH);
    analogWrite(pinSpeedMotor, map(value, 50, 1000, 0, 255));
  } else {
    digitalWrite(pin1Motor, LOW);
    digitalWrite(pin2Motor, LOW);
    digitalWrite(pinSpeedMotor, LOW);
  }
}

