#include <Wire.h>
#include <Servo.h>
#include <nRF24L01.h>
#include <printf.h>
#include <RF24.h>
#include <RF24_config.h>

const int MPU_addr = 0x68;
const float sqrt32 = sqrt(3)/2;

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

int xObj = 0;
int yObj = 0;

int vX;
int vY;
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
  // Centrale inertielle
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr, 14, true);
  AcX = Wire.read() << 8 | Wire.read();
  AcY = Wire.read() << 8 | Wire.read();
  AcZ = Wire.read() << 8 | Wire.read();
  Tmp = Wire.read() << 8 | Wire.read();
  GyX = Wire.read() << 8 | Wire.read();
  GyY = Wire.read() << 8 | Wire.read();
  GyZ = Wire.read() << 8 | Wire.read();
  Serial.print(AcX);
  Serial.print("\t");
  Serial.print(AcY);
  Serial.print("\t");
  Serial.print(AcZ);
  Serial.print("\t");
  //Serial.print(GyX);
  //Serial.print("\t");
  //Serial.print(GyY);
  //Serial.print("\t");
  //Serial.print(GyZ);
  //Serial.print("\t");
  //Serial.print(Tmp / 340.00 + 36.53); //equation for temperature in degrees C from datasheet
  Serial.println();
  
  //Radio
  if (radio.available()) {
    while (radio.available()) {
      radio.read(msg, 8);
    }
    Serial.print(msg[0]);
    Serial.print("\t");
    Serial.print(msg[1]);
    Serial.print("\t");
    Serial.print(msg[2]);
    Serial.print("\t");
    Serial.println(msg[3]);
  }
  
  //Asservissement
  
}
