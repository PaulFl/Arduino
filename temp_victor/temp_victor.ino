#include <SPI.h>
#include "Adafruit_MAX31855.h"



int MISOPin = 5;
int CLOCKPin = 7;
int CS1Pin = 6;
int CS2Pin = 4;

int tensionPin = A0;

Adafruit_MAX31855 thermo1(CLOCKPin, CS1Pin, MISOPin);
Adafruit_MAX31855 thermo2(CLOCKPin, CS2Pin, MISOPin);

float constantePasseBas = 0.2;

float thermo1PB = 20;
float thermo2PB = 20;

float thermo1Mesure;
float thermo2Mesure;

int mesureTension;
float tension;
float constanteTension = 0.0048828125; //A regler avec un voltmetre (5V / 1024 (10 bits))


void setup() {
  Serial.begin(9600);
  delay(100);
}

void loop() {
  //Thermo 1
  thermo1Mesure = thermo1.readCelsius();
  thermo1PB += constantePasseBas * (thermo1Mesure - thermo1PB);
  
  Serial.print("Themo 1 (C°): ");
  Serial.print(thermo1Mesure);
  Serial.print("\t");
  Serial.print("Themo 1 filtré(C°): ");
  Serial.print(thermo1PB);
  Serial.print("\t");

  //Thermo2
  thermo2Mesure = thermo2.readCelsius();
  thermo2PB += constantePasseBas * (thermo2Mesure - thermo2PB);
  
  Serial.print("Thermo 2 (C°): ");
  Serial.print(thermo2Mesure);
  Serial.print("\t");
  Serial.print("Themo 2 filtré(C°): ");
  Serial.print(thermo2PB);
  Serial.print("\t");

  //Tension
  mesureTension = analogRead(tensionPin);
  tension = constanteTension * mesureTension;
  Serial.print("Tension (V): ");
  Serial.print(tension);
  Serial.println(); //Saut à la ligne
  

  delay(5); //5 millisecondes
}
