#ifndef RBBH
#define RBBH

//Setup
#define MODE 2 //0 OFF, 1 Motor control with remote, 2 balance still, 3 playground, debug, tests
#define WIRELESSDEBUG 0 //not compatible with remote control of the robot

#define MOTORON 1  //Enable motors

//Debug options
#define DEBUGRADIO 0
#define DEBUGCI 1
#define DEBUGMOTORSPEED 0
#define DEBUG 0

//Pins
#define RADIOCEPIN 7
#define RADIOCSNPIN 8
#define MOTOR1PIN 10
#define MOTOR2PIN 9
#define MOTOR3PIN 6

//CI CSTS
#define ACXMAX 16000
#define ACXMIN -16000
#define ACYMAX 16000
#define ACYMIN -16000
#define ACXOFFSET 0
#define ACYOFFSET 0
#define GYXOFFSET 240
#define GYYOFFSET -200

float acLowPass = 0.09;

//Motors CSTS
#define MOTORNEUTRAL 95

//CSTS
const float Kp =15;
const float Ki = 0.3;
const float Kd = 0;

//Libs
#include <Arduino.h>
#include <Wire.h>
#include <Servo.h>
#include <nRF24L01.h>
#include <printf.h>
#include <RF24.h>
#include <RF24_config.h>
#include "Kalman.h"


//Prototypes
void readCIRaw();
void readRadio();
void calculateMotorSpeeds();
void calibration();
void initCI();
void readCI();

const int MPU_addr = 0x68;

const float sqrt32 = sqrt(3) / 2;

struct COORD {
  double acOffset;
  double gyOffset;
  double ac;
  double gy;
  double angle;
  double angle2;
  double acLowPass = 0;
  double target;
  double speed;
  double error = 0;
  double sumError = 0;
  Kalman kalman;
  Kalman kalman2;
  double angleKal;
  double angleKal2;
  double gyroRate;
  double angleCentered;
  double angleOffset;
  double gyroRateCentered;
  double gyroOffset;
};

struct MOTOR {
  short pin;
  Servo servo;
  int speed;
};

//Var
COORD x;
COORD y;
COORD z;

MOTOR motor1;
MOTOR motor2;
MOTOR motor3;

int Tmp;

int msg[4];
long int timer;
uint8_t i2cData[14];

RF24 radio(7, 8);
const uint64_t pipe = 0xE8E8F0F0E1LL;//White remote, ballance robot

int wirelessDebugArray[5]; //x acc, y acc, motor1, motor2, motor3

#endif
