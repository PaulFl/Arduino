#include <CapacitiveSensor.h>

const int ledPin = 13;      // led connected to digital pin 13
const int knockSensor = A0; //electet microphone
const int potentio = A1;// potentiometre

CapacitiveSensor capSensor = CapacitiveSensor(4,2);

unsigned long time;
int threshold;
int sensorThreshold = 50;
int sensorReading = 0;      // variable to store the value read from the sensor pin
boolean ledState = LOW;         // variable used to store the last LED status, to toggle the light

void setup() {
  pinMode(ledPin, OUTPUT); // declare the ledPin as as OUTPUT
  time = millis();  
}

void loop() {
  long sensorValue = capSensor.capacitiveSensor(30);
  // read the sensor and store it in the variable sensorReading:
  threshold = map(analogRead(potentio), 0, 1023, 915, 935);
  sensorReading = analogRead(knockSensor);
  // if the sensor reading is greater than the threshold:
  if (sensorReading >= threshold && millis()-time > 1000) {
    // toggle the status of the ledPin:
    ledState = !ledState;   
    time = millis();
    // update the LED pin itself:        
    digitalWrite(ledPin, ledState);
    // send the string "Knock!" back to the computer, followed by newline
  }
   if(sensorValue > sensorThreshold && millis()-time>1000) {
    // turn the LED on
    ledState = !ledState;
    time = millis();
    digitalWrite(ledPin, ledState);
  }
  delay(10);  // delay to avoid overloading the serial port buffer
}


