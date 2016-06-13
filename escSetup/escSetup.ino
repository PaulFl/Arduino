#include <Servo.h>

#define PIN 9
#define MIN 20

#define DELAY 3000

Servo esc;

void setup() {
  esc.attach(PIN);
  Serial.begin(9600);

  esc.write(180);
  delay(DELAY);
  esc.write(MIN);
}

void loop() {}
