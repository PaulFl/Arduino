const int pin = 8;
unsigned long value;

void setup() {
  Serial.begin(9600);
  Serial.println("Initialisation...");
  pinMode(pin, OUTPUT);
}

void loop() {
  value = pulseIn(pin, HIGH);
  Serial.println(value);
  delay(5);
}
