//Libs
#include <Wire.h>
#include <SPI.h>
#include "SdFat.h"
#include <string.h>

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
#define PPM_CHANS 5
#define IBUS_MAXCHANNELS 14
static uint16_t rcFailsafe[IBUS_MAXCHANNELS] = {  1500, 1500, 950, 1500, 2000, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500 };
#define FAILSAFELIMIT 1020
#define IBUS_BUFFSIZE 32    // Max iBus packet size (2 byte header, 14 channels x 2 bytes, 2 byte checksum)                                                                                                                             
static uint16_t rcValue[IBUS_MAXCHANNELS];
static uint16_t rcValueSafe[IBUS_MAXCHANNELS]; // read by interrupt handler. Data copied here in cli/sei block
static boolean rxFrameDone;
static boolean failsafe = 0;
uint16_t dummy;
static uint8_t ibusIndex = 0;
static uint8_t ibus[IBUS_BUFFSIZE] = {0};

//Prototypes
void read_CI();
void init_log_file();
void log_SD_card();
void write_to_SD(String log);
void setupRx();
void readRx();


void read_CI(){
    Wire.beginTransmission(CMPS12_ADDRESS);
    Wire.write(1); //Skip version
    Wire.endTransmission();
    
    Wire.requestFrom(CMPS12_ADDRESS, 30);
    Wire.read(); //Skip 8bit heading
    
    heading = Wire.read();
    heading <<= 8;
    heading += Wire.read();
    
    pitch = Wire.read();
    roll = Wire.read();
    
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
}

void init_log_file() {
    log_str = String("time;roll;pitch;heading;raw_x_acc;raw_y_acc;raw_z_acc;raw_x_gyro;raw_y_gyro;raw_z_gyro;calibration;channel_1;channel_2;channel_3;channel_4;channel_5;");
    write_to_SD(log_str);
}

void log_SD_card(){
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
    log_str += String(rcValue[0]) += ";";
    log_str += String(rcValue[1]) += ";";
    log_str += String(rcValue[2]) += ";";
    log_str += String(rcValue[3]) += ";";
    log_str += String(rcValue[4]);
    write_to_SD(log_str);
}

void write_to_SD(String log){
    data_file = SD.open(file_name + file_extension, FILE_WRITE);
    if (data_file){
        data_file.println(log);
        data_file.close();
    }
}

void setupRx(){
  uint8_t i;
  for (i = 0; i < PPM_CHANS; i++) { rcValue[i] = 1127; }
  Serial.begin(115200);
}

void readRx(){
  uint8_t i;
  uint16_t chksum, rxsum;

  rxFrameDone = false;

  uint8_t avail = Serial.available();
  
  if (avail)
  {
    digitalWrite(4, LOW);
    uint8_t val = Serial.read();
    // Look for 0x2040 as start of packet
    if (ibusIndex == 0 && val != 0x20) {
      return;
    }
    if (ibusIndex == 1 && val != 0x40) {
      ibusIndex = 0;
      return;
    }
 
    if (ibusIndex < IBUS_BUFFSIZE) ibus[ibusIndex] = val;
    ibusIndex++;

    if (ibusIndex == IBUS_BUFFSIZE)
    {
      ibusIndex = 0;
      chksum = 0xFFFF;
      for (i = 0; i < 30; i++)
        chksum -= ibus[i];

      rxsum = ibus[30] + (ibus[31] << 8);
      if (chksum == rxsum)
      {
        //Unrolled loop  for 10 channels - no need to copy more than needed.
        // MODIFY IF MORE CHANNELS NEEDED
        rcValue[0] = (ibus[ 3] << 8) + ibus[ 2];
        rcValue[1] = (ibus[ 5] << 8) + ibus[ 4];
        rcValue[2] = (ibus[ 7] << 8) + ibus[ 6];
        rcValue[3] = (ibus[ 9] << 8) + ibus[ 8];
        rcValue[4] = (ibus[11] << 8) + ibus[10];
        rcValue[5] = (ibus[13] << 8) + ibus[12];
        rcValue[6] = (ibus[15] << 8) + ibus[14];
        rcValue[7] = (ibus[17] << 8) + ibus[16];
        rcValue[8] = (ibus[19] << 8) + ibus[18];
        rcValue[9] = (ibus[21] << 8) + ibus[20];
        rxFrameDone = true;
        if (rcValue[0] < FAILSAFELIMIT && rcValue[1] < FAILSAFELIMIT &&
            rcValue[2] < FAILSAFELIMIT && rcValue[3] < FAILSAFELIMIT &&
            rcValue[4] < FAILSAFELIMIT && rcValue[5] < FAILSAFELIMIT ) 
        {
          failsafe = 1;
          cli(); // disable interrupts
          memcpy(rcValueSafe, rcFailsafe, IBUS_MAXCHANNELS * sizeof(uint16_t));
          sei();
          digitalWrite(13, HIGH);  //  Error - turn on error LED
        }
        else
        {
          // Now we need to disable interrupts to copy 16-bit values atomicly
          // Only copy needed signals (10 channels default)
          // MODIFY IF MORE CHANNELS NEEDED
          cli(); // disable interrupts.
          rcValueSafe[0] = rcValue[0];
          rcValueSafe[1] = rcValue[1];
          rcValueSafe[2] = rcValue[2];
          rcValueSafe[3] = rcValue[3];
          rcValueSafe[4] = rcValue[4];
          rcValueSafe[5] = rcValue[5];
          rcValueSafe[6] = rcValue[6];
          rcValueSafe[7] = rcValue[7];
          rcValueSafe[8] = rcValue[8];
          rcValueSafe[9] = rcValue[9];         
          sei();
        }
      } 
      return;
    }
  }
}

void setup() {
    Wire.begin();
    SD.begin(sd_card_pin);
    setupRx();
    delay(1000);
    write_to_SD(String("time;roll;pitch;heading;raw_x_acc;raw_y_acc;raw_z_acc;raw_x_gyro;raw_y_gyro;raw_z_gyro;calibration;channel_1;channel_2;channel_3;channel_4;channel_5"));
}

void loop() {
    read_CI();
    readRx();
    log_SD_card();
}
