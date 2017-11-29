#include <Wire.h>

const int MPU_addr = 0x68;

int AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;


void setup() {
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  Serial.begin(9600);
}
void loop() {
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
//  Serial.print(AcY);
//  Serial.print("\t");
//  Serial.print(AcZ);
//  Serial.print("\t");
  //Serial.print(GyX);
  //Serial.print("\t");
  //Serial.print(GyY);
  //Serial.print("\t");
  //Serial.print(GyZ);
  //Serial.print("\t");
  //Serial.print(Tmp / 340.00 + 36.53); //equation for temperature in degrees C from datasheet
  Serial.println();
  delay(5);
}
