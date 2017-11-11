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
#define ACXOFFSET 1025
#define ACYOFFSET 125
const float acLowPass = 0.2;

//Motors CSTS
#define MOTORNEUTRAL 95

//Libs
#include <Arduino.h>
#include <Wire.h>
#include <Servo.h>
#include <nRF24L01.h>
#include <printf.h>
#include <RF24.h>
#include <RF24_config.h>


//Prototypes
void readCI();
void readRadio();
void calculateMotorSpeeds();


const int MPU_addr = 0x68;

const float sqrt32 = sqrt(3) / 2;

struct COORD {
  int acOffset;
  int ac;
  int acMin;
  int acMax;
  int gy;
  float angle;
  long int acLowPass = 0;
  int target;
  int speed;
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

RF24 radio(7, 8);
const uint64_t pipe = 0xE8E8F0F0E1LL;//White remote, ballance robot

int wirelessDebugArray[5]; //x acc, y acc, motor1, motor2, motor3

#endif
