short motorOutput1 = 3;
short motorOutput2 = 5;
short speedControl = 9;

void setup() {
  // put your setup code here, to run once:
  pinMode(motorOutput1, OUTPUT);
  pinMode(motorOutput2, OUTPUT);
  pinMode(speedControl, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(motorOutput2, LOW);
  digitalWrite(motorOutput1, HIGH);
  for(int i=0; i<=255; i++){
    analogWrite(speedControl, i);
    delay(500);
  }

  digitalWrite(motorOutput1, LOW);
  digitalWrite(motorOutput2, HIGH);
  for(int i=0; i<=255; i++){
    analogWrite(speedControl, i);
    delay(500);
  }
}
