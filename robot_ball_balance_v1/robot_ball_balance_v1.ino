#define WIRELESSDEBUG 0 //not compatible with remote control of the robot
#define MODE 2 //0 OFF, 1 Motor control with remote, 2 balance still, 3 playground, debug, tests

#define MOTORON 0

#define AVERAGEPTS 10

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
#define ACXMIDDLE 0
#define ACYMIDDLE 0

//Libs
#include <Wire.h>
#include <Servo.h>
#include <nRF24L01.h>
#include <printf.h>
#include <RF24.h>
#include <RF24_config.h>

void readCI();
void readRadio();

const int MPU_addr = 0x68;

const float sqrt32 = sqrt(3) / 2;

struct COORD {
  int ac;
  int acMin;
  int acMax;
  int acMiddle;
  int gy;
  float angle;
  long int acAvgArray[AVERAGEPTS];
  long int acAvg = 0;
  long int acAvg2 = 0;
  int target;
  int speed;
};

struct MOTOR {
  short pin;
  Servo servo;
  int speed;
};

int maxMotorSpeed;

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


void setup() {
  //Serial setup
  Serial.begin(9600);
  Serial.println("Intitialisaton");

  //CI Setup
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  x.speed = 0;
  y.speed = 0;
  x.acMin = ACXMIN;
  x.acMax = ACXMAX;
  x.acMiddle = ACXMIDDLE;
  y.acMin = ACYMIDDLE;
  y.acMax = ACYMAX;
  y.acMiddle = ACYMIDDLE;
  x.target = x.acMiddle;
  y.target = y.acMiddle;

  //Motors setup
  motor1.pin = MOTOR1PIN;
  motor2.pin = MOTOR2PIN;
  motor3.pin = MOTOR3PIN;
  motor1.speed = 0;
  motor2.speed = 0;
  motor3.speed = 0;
  motor1.servo.attach(motor1.pin);
  motor2.servo.attach(motor2.pin);
  motor3.servo.attach(motor3.pin);
  maxMotorSpeed = x.acMax * sqrt(3);
  motor1.servo.write(95);
  motor2.servo.write(95);
  motor3.servo.write(95);

  //Radio Setup
  radio.begin();
  if (WIRELESSDEBUG) {
    radio.begin();
    radio.openWritingPipe(pipe);
  } else if (MODE == 1) {
    radio.begin();
    radio.openReadingPipe(1, pipe);
    radio.startListening();
  }
}

void loop() {
  readCI();

  switch (MODE) {
    case 1: //Motor control with remote
      readRadio();

      y.speed = msg[0]; //entre -100 et 100
      x.speed = msg[1]; //entre -100 et 100

      y.speed = 10;
      x.speed = 0;

      motor1.speed = y.speed;
      motor2.speed = -x.speed - y.speed / sqrt32;
      motor3.speed = +x.speed - y.speed / sqrt32;



      motor1.speed = map(motor1.speed, -100, 100, 95 - sqrt32 * 95, 95 + sqrt32 * 95);
      motor2.speed = map(motor2.speed, -100 - 100/sqrt32, +100 + 100/sqrt32 , 95 - 95 , 95 + 95);
      motor3.speed = map(motor3.speed, -100 - 100/sqrt32, +100 + 100/sqrt32 , 95 - 95 , 95 + 95);

        if (DEBUG) {
        Serial.print(x.speed);
        Serial.print("\t");
        Serial.print(y.speed);
        Serial.print("\t");
        Serial.print(motor1.speed);
        Serial.print("\t");
        Serial.print(motor2.speed);
        Serial.print("\t");
        Serial.print(motor3.speed);
        Serial.println("\t");

      }
    

      break;

    case 2: //Balance still
      //Asservissement
      x.speed = x.target - x.acAvg;
      y.speed = y.target - y.acAvg;

      motor1.speed = x.speed / sqrt32 + y.speed / sqrt32;
      motor2.speed = -x.speed / sqrt32 + y.speed / sqrt32;
      motor3.speed = x.speed;

      motor1.speed = map(motor1.speed, -maxMotorSpeed, maxMotorSpeed, 0, 180);
      motor2.speed = map(motor2.speed, -maxMotorSpeed, maxMotorSpeed, 0, 180);
      motor3.speed = map(motor3.speed, -maxMotorSpeed, maxMotorSpeed, 0, 180);
      break;

    case 3: //Playground, tests, debug
      // +x:  95-v, moteur3 95+v
      // +y moteur1 95+v, moteur2 95-v/sqrt32, moteur3 95-v/sqrt32
      motor1.speed = 95 + 5;
      motor2.speed = 95 - 5 / sqrt32;
      motor3.speed = 95 - 5 / sqrt32;
      break;

    default: //OFF
      motor1.speed = 180;
      motor2.speed = 0;
      motor3.speed = 95;
      break;
  }


  if (DEBUGMOTORSPEED) {
    Serial.print(motor1.speed);
    Serial.print("\t");
    Serial.print(motor2.speed);
    Serial.print("\t");
    Serial.print(motor3.speed);
    Serial.print("\t");
    Serial.println();
  }

  if (MOTORON) {
    motor1.servo.write(motor1.speed);
    motor2.servo.write(motor2.speed);
    motor3.servo.write(motor3.speed);
  }
}

void readCI() {
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr, 14, true);

  x.ac = Wire.read() << 8 | Wire.read();
  for (int i = 0; i < AVERAGEPTS - 1; i++) x.acAvgArray[i] = x.acAvgArray[i + 1];
  x.acAvgArray[AVERAGEPTS - 1] = x.ac;
  x.acAvg = 0;
  for (int i = 0; i < AVERAGEPTS; i++) x.acAvg += x.acAvgArray[i];
  x.acAvg /= AVERAGEPTS;
  x.acAvg2 += 0.2 * (x.ac - x.acAvg2);

  y.ac = Wire.read() << 8 | Wire.read();
  for (int i = 0; i < AVERAGEPTS - 1; i++) y.acAvgArray[i] = y.acAvgArray[i + 1];
  y.acAvgArray[AVERAGEPTS - 1] = y.ac;
  y.acAvg = 0;
  for (int i = 0; i < AVERAGEPTS; i++) y.acAvg += y.acAvgArray[i];
  y.acAvg /= AVERAGEPTS;

  z.ac = Wire.read() << 8 | Wire.read();
  Tmp = Wire.read() << 8 | Wire.read();
  x.gy = Wire.read() << 8 | Wire.read();
  y.gy = Wire.read() << 8 | Wire.read();
  z.gy = Wire.read() << 8 | Wire.read();

  x.angle = asin(float(x.acAvg) / float(x.acMax)) * 180 / PI;
  y.angle = asin(float(y.acAvg) / float(y.acMax)) * 180 / PI;

  if (DEBUGCI) {
    Serial.print(x.ac);
    Serial.print("\t");
   // Serial.print(x.acAvg);
//    Serial.print("\t");
//    Serial.print(x.acAvg2);
//    Serial.print("\t");
    //Serial.print(x.angle);
    //Serial.print("\t");
    Serial.print(y.ac);
    Serial.print("\t");
    //Serial.print(y.acAvg);
    //Serial.print("\t");
    //Serial.print(z.ac);
    //Serial.print("\t");
    //Serial.print(x.gy);
    //Serial.print("\t");
    //Serial.print(y.gy);
    //Serial.print("\t");
    //Serial.print(z.gy);
    //Serial.print("\t");
    //Serial.print(Tmp / 340.00 + 36.53); //equation for temperature in degrees C from datasheet
    Serial.println();
  }

  if (WIRELESSDEBUG) {
    wirelessDebugArray[0] = x.ac;
    wirelessDebugArray[1] = y.ac;
    wirelessDebugArray[2] = motor1.speed;
    wirelessDebugArray[3] = motor2.speed;
    wirelessDebugArray[4] = motor3.speed;

    radio.write(wirelessDebugArray, 10);
  }
}

void readRadio() {
  if (radio.available()) {
    while (radio.available()) {
      radio.read(msg, 8);
    }

    msg[0] = map(msg[0], 0, 180, -100, 100);
    msg[1] = map(msg[1], 0, 100, -100, 100);

    if (DEBUGRADIO) {
      Serial.print(msg[0]);
      Serial.print("\t");
      Serial.print(msg[1]);
      Serial.print("\t");
      //  Serial.print(msg[2] * 100);
      //  Serial.print("\t");
      // Serial.print(msg[3] * 100);
      //  Serial.print("\t");
      // Serial.println();
    }
  }
}
