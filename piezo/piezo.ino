short piezo = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("Initialisation...");
  pinMode(piezo, INPUT);
}

void loop() {
  Serial.println(analogRead(piezo));
  delay(10);
}
