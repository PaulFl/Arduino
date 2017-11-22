#include "robot_ball_balance_v1.h"

void calculateMotorSpeeds() { //calculate motor1,2,3.speed from x,y.speed
  motor1.speed = y.speed;
  motor2.speed = -x.speed * sqrt32 - y.speed / 2.0;
  motor3.speed = +x.speed * sqrt32 - y.speed / 2.0;

  motor1.speed = map(motor1.speed, -100, 100, MOTORNEUTRAL - MOTORNEUTRAL, MOTORNEUTRAL + MOTORNEUTRAL);
  motor2.speed = map(motor2.speed, -100, 100, MOTORNEUTRAL - MOTORNEUTRAL, MOTORNEUTRAL + MOTORNEUTRAL);
  motor3.speed = map(motor3.speed, -100, 100, MOTORNEUTRAL - MOTORNEUTRAL, MOTORNEUTRAL + MOTORNEUTRAL);
}

void readCI() {
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr, 14, true);

  x.ac = Wire.read() << 8 | Wire.read();
  x.ac = -x.ac;
  x.ac += x.acOffset;
  x.acLowPass += acLowPass * (x.ac - x.acLowPass); //Low pass filter

  y.ac = Wire.read() << 8 | Wire.read();
  y.ac += y.acOffset;
  y.acLowPass += acLowPass * (y.ac - y.acLowPass); //Low pass filter

  z.ac = Wire.read() << 8 | Wire.read();
  Tmp = Wire.read() << 8 | Wire.read();

  y.gy = Wire.read() << 8 | Wire.read();
  y.gy += y.gyOffset;
  y.gy *= (-1);

  x.gy = Wire.read() << 8 | Wire.read();
  x.gy += x.gyOffset;
  x.gy *= (-1);

  z.gy = Wire.read() << 8 | Wire.read();

  x.angle = asin(float(x.acLowPass) / float(ACXMAX)) * 180 / PI;
  y.angle = asin(float(y.acLowPass) / float(ACYMAX)) * 180 / PI;

  if (DEBUGCI) {
    //    Serial.print(x.ac);
    //    Serial.print("\t");
    Serial.print(x.acLowPass);
    Serial.print("\t");
    //Serial.print(x.angle);
    //Serial.print("\t");
    //    Serial.print(y.ac);
    //    Serial.print("\t");
    Serial.print(y.acLowPass);
    Serial.print("\t");
    //Serial.print(z.ac);
    //Serial.print("\t");
    //    Serial.print(x.gy);
    //    Serial.print("\t");
    //    Serial.print(y.gy);
    //    Serial.print("\t");
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

    msg[0] = map(msg[0], 0, 180, -100, 100); //x axis
    msg[1] = map(msg[1], 0, 100, -100, 100); //y axis

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

void calibration() {
  for (int i = 0; i <= 10; i++) {
    readCI();
    delay(10);
    if (DEBUG) {
      printDebug();
    }
  }
  float tmpVar = acLowPass;
  acLowPass = 0.05;
  for (int i = 0; i <= 700; i++) {
    readCI();
    if (DEBUG) {
      printDebug();
    }
  }
  acLowPass = tmpVar;
  x.acOffset = -x.acLowPass;
  y.acOffset = -y.acLowPass;
  for (int i = 0; i <= 100; i++) {
    readCI();
    if (DEBUG) {
      printDebug();
    }
  }
}

void printDebug() {
  Serial.print(x.acLowPass);
  Serial.print("\t");
  Serial.print(y.acLowPass);
  Serial.print("\t");
  Serial.print(motor1.speed);
  Serial.print("\t");
  Serial.print(motor2.speed);
  Serial.print("\t");
  Serial.print(motor3.speed);
  Serial.print("\t");
  Serial.println();
}
