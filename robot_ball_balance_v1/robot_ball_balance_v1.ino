#define DEBUGRADIO 1
#define DEBUGCI 1
#define DEBUGMOTORSPEED 1

#define AVERAGEPTS 5

#include <Wire.h>
#include <Servo.h>
#include <nRF24L01.h>
#include <printf.h>
#include <RF24.h>
#include <RF24_config.h>

void lireCentraleInertielle();
void lireRadio();

const int AXMax = 14000;
const int AXMin = -14000;
const int AYMin = -14000;
const int AYMax = 14000;

const int AXMiddle = 0;
const int AYMiddle = 0;

int avgAxArray[AVERAGEPTS];
int avgAx = 0;
int avgAyArray[AVERAGEPTS];
int avgAy = 0;

const int MPU_addr = 0x68;
const float sqrt32 = sqrt(3) / 2;

int AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;
short pin_moteur1 = 6;
short pin_moteur2 = 9;
short pin_moteur3 = 10;

Servo moteur1;
Servo moteur2;
Servo moteur3;

int msg[4];
RF24 radio(7, 8);
const uint64_t pipe = 0xE8E8F0F0E1LL;//White remote

int xObj = AXMiddle;
int yObj = AYMiddle;

int vX = 0;
int vY = 0;
int vitesseMoteur1 = 0;
int vitesseMoteur2 = 0;
int vitesseMoteur3 = 0;


void setup() {
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  moteur1.attach(pin_moteur1);
  moteur2.attach(pin_moteur2);
  moteur3.attach(pin_moteur3);
  radio.begin();
  radio.openReadingPipe(1, pipe);
  radio.startListening();
  Serial.begin(9600);
}
void loop() {
  lireCentraleInertielle();
  lireRadio();

  //Asservissement
  vX = xObj - AcX;
  vY = yObj - AcY;

  vitesseMoteur1 = vX / sqrt32 + vY / sqrt32;
  vitesseMoteur2 = -vX / sqrt32 + vY / sqrt32;
  vitesseMoteur3 = vX;

  vitesseMoteur1 = map(vitesseMoteur1, -100 * sqrt32, 100 * sqrt32, 0, 180);
  vitesseMoteur2 = map(vitesseMoteur2, -100 * sqrt32, 100 * sqrt32, 0, 180);
  vitesseMoteur3 = map(vitesseMoteur3, -100 * sqrt32, 100 * sqrt32, 0, 180);
  if (DEBUGMOTORSPEED) {
    Serial.print(vitesseMoteur1);
    Serial.print("\t");
  }

  moteur1.write(vitesseMoteur1);
  moteur2.write(vitesseMoteur2);
  moteur3.write(vitesseMoteur3);
  Serial.println();
  delay(2);
}

void lireCentraleInertielle() {
  // Centrale inertielle
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr, 14, true);
  AcX = Wire.read() << 8 | Wire.read();
  AcX = map(AcX, AXMin, AXMax, -100, 100);
  for (int i = 0; i < AVERAGEPTS - 1; i++) {
    avgAxArray[i] = avgAxArray[i + 1];
  }
  avgAxArray[AVERAGEPTS - 1] = AcX;
  avgAx = 0;
  for (int i = 0; i < AVERAGEPTS; i++) {
    avgAx += avgAxArray[i];
  }
  avgAx /= AVERAGEPTS;
  AcY = Wire.read() << 8 | Wire.read();
  AcY = map(AcY, AYMin, AYMax, -100, 100);
  for (int i = 0; i < AVERAGEPTS - 1; i++) {
    avgAyArray[i] = avgAyArray[i + 1];
  }
  avgAyArray[AVERAGEPTS - 1] = AcY;
  avgAy = 0;
  for (int i = 0; i < AVERAGEPTS; i++) {
    avgAy += avgAyArray[i];
  }
  avgAy /= AVERAGEPTS;
  AcZ = Wire.read() << 8 | Wire.read();
  Tmp = Wire.read() << 8 | Wire.read();
  GyX = Wire.read() << 8 | Wire.read();
  GyY = Wire.read() << 8 | Wire.read();
  GyZ = Wire.read() << 8 | Wire.read();
  if (DEBUGCI) {
    Serial.print(avgAx);
    Serial.print("\t");
    Serial.print(avgAy);
    Serial.print("\t");
    //Serial.print(AcX);
    //Serial.print("\t");
    //Serial.print(AcY);
    //Serial.print("\t");
    //Serial.print(AcZ);
    //Serial.print("\t");
    //Serial.print(GyX);
    //Serial.print("\t");
    //Serial.print(GyY);
    //Serial.print("\t");
    //Serial.print(GyZ);
    //Serial.print("\t");
    //Serial.print(Tmp / 340.00 + 36.53); //equation for temperature in degrees C from datasheet
  }
}

void lireRadio() {
  //Radio
  if (radio.available()) {
    while (radio.available()) {
      radio.read(msg, 8);
    }
    msg[0] = map(msg[0], 0, 100, -100, 100);
    msg[1] = map(msg[0], 0, 180, -100, 100);
    if (DEBUGRADIO) {
      Serial.print(msg[0]);
      Serial.print("\t");
      Serial.print(msg[1]);
      Serial.print("\t");
      Serial.print(msg[2]);
      Serial.print("\t");
      Serial.print(msg[3]);
      Serial.print("\t");
    }
  }
}

