
#include <CapacitiveSensor.h>

// create an instance of the library
// pin 4 sends electrical energy
// pin 2 senses senses a change
CapacitiveSensor capSensor = CapacitiveSensor(4,6);

// threshold for turning the lamp on
int sensorThreshold = 50;

boolean ledState = LOW;

unsigned long time;

// pin the LED is connected to
const int ledPin = 13;


void setup() {
  // open a serial connection
  Serial.begin(9600);
  // set the LED pin as an output
  pinMode(ledPin, OUTPUT);
  time = millis();
}

void loop() {
  // store the value reported by the sensor in a variable
  long sensorValue = capSensor.capacitiveSensor(30);

  // print out the sensor value
  Serial.println(sensorValue); 

  // if the value is greater than the threshold
  if(sensorValue > sensorThreshold && millis()-time>1000) {
    // turn the LED on
    ledState = !ledState;
    time = millis();
    digitalWrite(ledPin, ledState);
  }
 

  delay(10);
}



