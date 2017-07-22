void setup() {
  // put your setup code here, to run once:
pinMode(9, OUTPUT);
pinMode(11, OUTPUT);
pinMode(A0, OUTPUT);
pinMode(A2, OUTPUT);
digitalWrite(9, 1);
digitalWrite(11, 1);
digitalWrite(A0, 1);
digitalWrite(A2, 1);
pinMode(10, INPUT_PULLUP);
pinMode(12, INPUT_PULLUP);
pinMode(A1, INPUT_PULLUP);
pinMode(A3, INPUT_PULLUP);
}

void loop() {
  // put your main code here, to run repeatedly:
digitalWrite(9, digitalRead(10));
digitalWrite(11, digitalRead(12));
digitalWrite(A0, digitalRead(A1));
digitalWrite(A2, digitalRead(A3));
}
