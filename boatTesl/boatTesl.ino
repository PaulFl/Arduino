void setup() {
  // put your setup code here, to run once:
pinMode(8, INPUT);
Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
delay(100);
Serial.println(pulseIn(8, HIGH));

}
