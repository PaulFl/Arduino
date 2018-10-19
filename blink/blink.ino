void setup(){
  pinMode(9, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(A0, OUTPUT);
  pinMode(A2, OUTPUT);

  digitalWrite(9,1);
  delay(1000);
  digitalWrite(11,1);
  delay(1000);
  digitalWrite(A0, 1);
  delay(1000);
  digitalWrite(A2, 1);

  pinMode(0, OUTPUT);
  pinMode(1, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(8, OUTPUT);

  delay(1000);
  digitalWrite(1, 1);
  digitalWrite(5, 1);
  delay(1000);
  digitalWrite(2, 1);
  delay(1000);
  digitalWrite(6, 1);
  delay(1000);
  digitalWrite(3,1);
  delay(1000);
  digitalWrite(7,1);
  delay(1000);
  digitalWrite(4,1);
  delay(1000);
  digitalWrite(8,1);
  delay(1000);
}

void loop(){}
