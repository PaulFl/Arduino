#include "robot_ball_balance_v1.h"

void setup() {
  //Serial setup
  Serial.begin(9600);
  Serial.println("Intitialisaton");

  //CI Setup
  //  Wire.begin();
  //  Wire.beginTransmission(MPU_addr);
  //  Wire.write(0x6B);  // PWR_MGMT_1 register
  //  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  //  Wire.endTransmission(true);
  //  x.speed = 0;
  //  y.speed = 0;
  //  x.target = 0;
  //  y.target = 0;
  //  x.acOffset = ACXOFFSET;
  //  y.acOffset = ACYOFFSET;
  //  x.gyOffset = GYXOFFSET;
  //  y.gyOffset = GYYOFFSET;

  initCI();

  //Motors setup
  motor1.pin = MOTOR1PIN;
  motor2.pin = MOTOR2PIN;
  motor3.pin = MOTOR3PIN;
  motor1.speed = MOTORNEUTRAL;
  motor2.speed = MOTORNEUTRAL;
  motor3.speed = MOTORNEUTRAL;
  motor1.servo.attach(motor1.pin);
  motor2.servo.attach(motor2.pin);
  motor3.servo.attach(motor3.pin);
  motor1.servo.write(motor1.speed);
  motor2.servo.write(motor2.speed);
  motor3.servo.write(motor3.speed);

  x.angleOffset = 4.5;
  y.angleOffset = 1.2;

  x.gyroOffset = 1;
  y.gyroOffset = -1.3;

  //Radio Setup
  //  radio.begin();
  //  if (WIRELESSDEBUG) {
  //    radio.begin();
  //    radio.openWritingPipe(pipe);
  //  } else if (MODE == 1) {
  //    radio.begin();
  //    radio.openReadingPipe(1, pipe);
  //    radio.startListening();
  //  }
  //
  //  calibration();
}

void loop() {
  //readCIRaw();
  delay(2);
  readCI();
  if (DEBUGCI) {
   // Serial.print(x.angle);
    Serial.print("\t");
    Serial.print(x.angleCentered);
    Serial.print("\t");
   // Serial.print(y.angle);
    Serial.print("\t");
    Serial.print(y.angleCentered);
    Serial.println();
  }


  switch (MODE) {
    case 1: //Motor control with remote
      readRadio();

      y.speed = msg[0]; //entre -100 et 100
      x.speed = msg[1]; //entre -100 et 100

      calculateMotorSpeeds();

      break;

    case 2: //Balance still

      x.error = x.angleCentered;
      y.error = y.angleCentered;

      if (abs(x.error) < 0.3){
        x.sumError = 0;
      }
      if (abs(y.error) < 0.3){
        y.sumError = 0;
      }

      
      x.sumError += x.error * 2; //dt ~2
      y.sumError += y.error * 2;
      

      x.speed = -(Kp * x.error + Ki * x.sumError + Kd * x.gyroRateCentered);
      y.speed = -(Kp * y.error + Ki * y.sumError + Kd * y.gyroRateCentered);

      calculateMotorSpeeds();

      if (DEBUG) {
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
