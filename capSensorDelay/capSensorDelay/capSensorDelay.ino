#include "Arduino.h"
#include "CapacitiveSensor.h"

CapacitiveSensor cs_4_2 = CapacitiveSensor(4,2);

void setup() {
    Serial.begin(9600);
    pinMode(13, OUTPUT);
}

void loop() {
    digitalWrite(13, HIGH);
    delay(100);
    digitalWrite(13, LOW);
    long total1 =  cs_4_2.capacitiveSensor(10);
    Serial.println(total1);
}