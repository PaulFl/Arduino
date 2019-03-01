/*****************************************
      CMPS12 I2C example for Arduino
         By James Henderson, 2014
*****************************************/

#include <Wire.h>

#define CMPS12_ADDRESS 0x60  // Address of CMPS12 shifted right one bit for arduino wire library
#define ANGLE_8  1           // Register to read 8bit angle from

unsigned char high_byte, low_byte, angle8;
char pitch, roll;
unsigned int angle16;

void setup()
{
  Serial.begin(9600);  // Start serial port
  Wire.begin();
  pinMode(13, OUTPUT);
  pinMode(6, OUTPUT);
}

void loop()
{

  Wire.beginTransmission(CMPS12_ADDRESS);
  Wire.write(1); //Skip version
  Wire.endTransmission();

  Wire.requestFrom(CMPS12_ADDRESS, 30);
  Wire.read(); //Skip 8bit heading

  int heading = Wire.read();
  heading <<= 8;
  heading += Wire.read();

  int roll = Wire.read();
  int pitch = Wire.read();

  for (int i = 0; i < 6; i++) { //Skip raw magnetometer
    Wire.read();
  }

  int raw_x_acc = Wire.read();
  raw_x_acc <<= 8;
  raw_x_acc += Wire.read();

  int raw_y_acc = Wire.read();
  raw_y_acc <<= 8;
  raw_y_acc += Wire.read();

  int raw_z_acc = Wire.read();
  raw_z_acc <<= 8;
  raw_z_acc += Wire.read();

  int raw_x_gyro = Wire.read();
  raw_x_gyro <<= 8;
  raw_x_gyro += Wire.read();

  int raw_y_gyro = Wire.read();
  raw_y_gyro <<= 8;
  raw_y_gyro += Wire.read();

  int raw_z_gyro = Wire.read();
  raw_z_gyro <<= 8;
  raw_z_gyro += Wire.read();

  for (int i = 0; i < 6; i++) { //Skip temp and pitch
    Wire.read();
  }

  int calibration = Wire.read();

  if (pitch > 100) {
    pitch = pitch - 255;
  }
  if (roll > 125) {
    roll = 77 + roll - 179;
  }
  Serial.print("roll");
  Serial.println(roll);



}
