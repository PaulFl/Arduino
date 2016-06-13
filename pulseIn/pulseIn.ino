const int pin = 5;
unsigned long value;

void setup() {
  Serial.begin(9600);
  Serial.println("Initialisation...");
}

void loop() {
  value = pulseIn(pin, HIGH);
  Serial.println(value);
  delay(5);
}
