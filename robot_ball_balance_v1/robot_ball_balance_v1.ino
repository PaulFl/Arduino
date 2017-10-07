#define MODE 0 //0 OFF, 1 Motor control with remote, 2 balance still

#define AVERAGEPTS 10

//Debug options
#define DEBUGRADIO 0
#define DEBUGCI 0
#define DEBUGMOTORSPEED 0

//Pins
#define RADIOCEPIN 7
#define RADIOCSNPIN 8
#define MOTOR1PIN 6
#define MOTOR2PIN 9
#define MOTOR3PIN 10

//Libs
#include <Wire.h>
#include <Servo.h>
#include <nRF24L01.h>
#include <printf.h>
#include <RF24.h>
#include <RF24_config.h>

void readCI();
void readRadio();

const int AXMax = 16000;
const int AXMin = -16000;
const int AYMin = -16000;
const int AYMax = 16000;
const int AXMiddle = 0;
const int AYMiddle = 0;

const int MPU_addr = 0x68;

const float sqrt32 = sqrt(3) / 2;

struct COORD {
  int ac;
  int gy;
  int angle;
  long int avgArray[AVERAGEPTS];
  long int avg = 0;
  int target;
  int speed;
};

struct MOTOR {
  short pin;
  Servo servo;
  int speed;
};

COORD x;
COORD y;
COORD z;

MOTOR motor1;
MOTOR motor2;
MOTOR motor3;

int Tmp;

int msg[4];

RF24 radio(7, 8);
const uint64_t pipe = 0xE8E8F0F0E1LL;//White remote


void setup() {
  //Serial setup
  Serial.begin(9600);
  Serial.println("Intitialisaton");

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

  //CI Setup
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  x.target = AXMiddle;
  y.target = AYMiddle;
  x.speed = 0;
  y.speed = 0;

  //Radio Setup
  radio.begin();
  radio.openReadingPipe(1, pipe);
  radio.startListening();
}

void loop() {
  readCI();
  readRadio();

  switch (MODE) {
    case 1: //Motor control with remote
      x.speed = msg[0];
      y.speed = msg[1];

      motor1.speed = x.speed / sqrt32 + y.speed / sqrt32;
      motor2.speed = -x.speed / sqrt32 + y.speed / sqrt32;
      motor3.speed = x.speed;

      motor1.speed = map(motor1.speed, -100 * sqrt(3), 100 * sqrt(3), 0, 180);
      motor2.speed = map(motor2.speed, -100 * sqrt(3), 100 * sqrt(3), 0, 180);
      motor3.speed = map(motor3.speed, -100 * sqrt(3), 100 * sqrt(3), 0, 180);
      break;

    case 2: //Balance still
      //Asservissement
      x.speed = x.target - x.avg;
      y.speed = y.target - y.avg;

      motor1.speed = x.speed / sqrt32 + y.speed / sqrt32;
      motor2.speed = -x.speed / sqrt32 + y.speed / sqrt32;
      motor3.speed = x.speed;

      motor1.speed = map(motor1.speed, -27700, 27700, 0, 180);
      motor2.speed = map(motor2.speed, -27700, 27700, 0, 180);
      motor3.speed = map(motor3.speed, -27700, 27700, 0, 180);
      break;

    default: //OFF
      motor1.speed = 0;
      motor2.speed = 0;
      motor3.speed = 0;
      break;
  }


  if (DEBUGMOTORSPEED) {
    Serial.print(motor1.speed);
    Serial.print("\t");
    Serial.println();
  }

  motor1.servo.write(motor1.speed);
  motor2.servo.write(motor2.speed);
  motor3.servo.write(motor3.speed);
}

void readCI() {
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr, 14, true);

  x.ac = Wire.read() << 8 | Wire.read();
  for (int i = 0; i < AVERAGEPTS - 1; i++) x.avgArray[i] = x.avgArray[i + 1];
  x.avgArray[AVERAGEPTS - 1] = x.ac;
  x.avg = 0;
  for (int i = 0; i < AVERAGEPTS; i++) x.avg += x.avgArray[i];
  x.avg /= AVERAGEPTS;

  y.ac = Wire.read() << 8 | Wire.read();
  for (int i = 0; i < AVERAGEPTS - 1; i++) y.avgArray[i] = y.avgArray[i + 1];
  y.avgArray[AVERAGEPTS - 1] = y.ac;
  y.avg = 0;
  for (int i = 0; i < AVERAGEPTS; i++) y.avg += y.avgArray[i];
  y.avg /= AVERAGEPTS;

  z.ac = Wire.read() << 8 | Wire.read();
  Tmp = Wire.read() << 8 | Wire.read();
  x.gy = Wire.read() << 8 | Wire.read();
  y.gy = Wire.read() << 8 | Wire.read();
  z.gy = Wire.read() << 8 | Wire.read();

  if (DEBUGCI) {
    Serial.print(x.ac);
    Serial.print("\t");
    Serial.print(x.avg);
    Serial.print("\t");
    Serial.print(y.ac);
    Serial.print("\t");
    Serial.print(y.avg);
    Serial.print("\t");
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
      Serial.print(msg[2] * 100);
      Serial.print("\t");
      Serial.print(msg[3] * 100);
      Serial.print("\t");
      Serial.println();
    }
  }
}
