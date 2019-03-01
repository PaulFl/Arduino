//Libs
#include <Wire.h>
#include <SPI.h>
#include "SdFat.h"
#include <string.h>
#include "FlySkyIBus.h"

#define CMPS12_ADDRESS 0x60

//****************** USER Settings
//Pins
short sd_card_pin = 4;

//File name
String file_name = "flight_log";
//File extension
String file_extension = ".csv";

//******************

//Data
int roll;
int pitch;
int heading;
int raw_x_acc;
int raw_y_acc;
int raw_z_acc;
int raw_x_gyro;
int raw_y_gyro;
int raw_z_gyro;
int calibration;

//Objs
SdFat SD;
File data_file;
String log_str;

//IBUS reading                                                                                                                            
static uint16_t rcValue[6];


//Prototypes
void read_CI();
void init_log_file();
void log_SD_card();
void write_to_SD(String log);
void setupRx();
void readRx();


void read_CI() {
  Wire.beginTransmission(CMPS12_ADDRESS);
  Wire.write(1); //Skip version
  Wire.endTransmission();

  Wire.requestFrom(CMPS12_ADDRESS, 30);
  Wire.read(); //Skip 8bit heading

  heading = Wire.read();
  heading <<= 8;
  heading += Wire.read();

  roll = Wire.read();
  pitch = Wire.read();

  for (int i = 0; i < 6; i++) { //Skip raw magnetometer
    Wire.read();
  }

  raw_x_acc = Wire.read();
  raw_x_acc <<= 8;
  raw_x_acc += Wire.read();

  raw_y_acc = Wire.read();
  raw_y_acc <<= 8;
  raw_y_acc += Wire.read();

  raw_z_acc = Wire.read();
  raw_z_acc <<= 8;
  raw_z_acc += Wire.read();

  raw_x_gyro = Wire.read();
  raw_x_gyro <<= 8;
  raw_x_gyro += Wire.read();

  raw_y_gyro = Wire.read();
  raw_y_gyro <<= 8;
  raw_y_gyro += Wire.read();

  raw_z_gyro = Wire.read();
  raw_z_gyro <<= 8;
  raw_z_gyro += Wire.read();

  for (int i = 0; i < 6; i++) { //Skip temp and pitch
    Wire.read();
  }

  calibration = Wire.read();

  if (pitch > 100) {
    pitch = pitch - 255;
  }
  if (roll > 125) {
    roll = 77 + roll - 179;
  }
}

void init_log_file() {
  log_str = String("time;roll;pitch;heading;raw_x_acc;raw_y_acc;raw_z_acc;raw_x_gyro;raw_y_gyro;raw_z_gyro;calibration;channel_1;channel_2;channel_3;channel_4;channel_5;");
  write_to_SD(log_str);
}

void log_SD_card() {
  log_str = String(millis()) + ";";
  log_str += String(roll) + ";";
  log_str += String(pitch) += ";";
  log_str += String(heading) += ";";
  log_str += String(raw_x_acc) += ";";
  log_str += String(raw_y_acc) += ";";
  log_str += String(raw_z_acc) += ";";
  log_str += String(raw_x_gyro) += ";";
  log_str += String(raw_y_gyro) += ";";
  log_str += String(raw_z_gyro) += ";";
  log_str += String(calibration) += ";";
 // log_str += String(rcValue[0]) += ";";
 // log_str += String(rcValue[1]) += ";";
 // log_str += String(rcValue[2]) += ";";
 // log_str += String(rcValue[3]) += ";";
  log_str += String(rcValue[4]);
  write_to_SD(log_str);
}

void write_to_SD(String log) {
  data_file = SD.open(file_name + file_extension, FILE_WRITE);
  if (data_file) {
    data_file.println(log);
    data_file.close();
  }
}

void setup() {
  Wire.begin();
  SD.begin(sd_card_pin);
  //IBus.begin(Serial);
  pinMode(2, INPUT);
  delay(1000);
  write_to_SD(String("time;roll;pitch;heading;raw_x_acc;raw_y_acc;raw_z_acc;raw_x_gyro;raw_y_gyro;raw_z_gyro;calibration;channel_1;channel_2;channel_3;channel_4;channel_5"));
}

void loop() {
  read_CI();
//  rcValue[0] = IBus.readChannel(0);
//  rcValue[1] = IBus.readChannel(1);
//  rcValue[2] = IBus.readChannel(2);
//  rcValue[3] = IBus.readChannel(3);
  rcValue[4] = pulseIn(2, HIGH);
  log_SD_card();
}
