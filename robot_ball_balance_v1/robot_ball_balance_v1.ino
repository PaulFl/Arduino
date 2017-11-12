#include "robot_ball_balance_v1.h"

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
  x.target = 0;
  y.target = 0;

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
  motor1.servo.write(MOTORNEUTRAL);
  motor2.servo.write(MOTORNEUTRAL);
  motor3.servo.write(MOTORNEUTRAL);

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

      calculateMotorSpeeds();

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
      readCI();

      x.error = x.acLowPass - x.target;
      y.error = y.acLowPass - y.target;

      x.sumError += x.error;
      y.sumError += y.error;

      x.dError = x.error - x.lastError;
      y.dError = y.error - y.lastError;

      x.speed = Kc * (Kp * x.error + Ki * x.sumError + Kd * x.dError);
      y.speed = Kc * (Kp * y.error + Ki * y.sumError + Kd * y.dError);

      x.lastError = x.error;
      y.lastError = y.error;

      calculateMotorSpeeds();

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

    case 3:
      break;

    default: //OFF
      motor1.speed = 180;
      motor2.speed = 0;
      motor3.speed = MOTORNEUTRAL;
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
