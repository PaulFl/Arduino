short receivePin = 2;
short ledPin = 13;
unsigned int value;
bool led = true;
bool switched = false;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(57600);
  pinMode(receivePin, INPUT);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
}

void loop() {
  // put your main code here, to run repeatedly:
  value = pulseIn(receivePin, HIGH);
  
  Serial.println(value);
}
