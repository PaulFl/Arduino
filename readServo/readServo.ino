short receivePin = 10;
short ledPin = 13;
unsigned int value;
bool led = true;
bool switched = false;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(receivePin, INPUT);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
}

void loop() {
  // put your main code here, to run repeatedly:
  value = pulseIn(receivePin, HIGH);
  if (value < 1000 && !switched) {
    switched = true;
    led = !led;
    digitalWrite(ledPin, led);
  }
  else if (value > 1000 && switched) {
    switched = false;
  }
  Serial.println(value);
}
