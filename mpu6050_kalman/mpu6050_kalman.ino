#include <Wire.h>
#include "Kalman.h" // Source: https://github.com/TKJElectronics/KalmanFilter


Kalman kalmanX;
Kalman kalmanY;

Kalman kalmanX2;
Kalman kalmanY2;

/* IMU Data */
double accX, accY, accZ;
double gyroX, gyroY, gyroZ;
int16_t tempRaw;


double yAngleKal2, xAngleKal; // Calculated angle using a Kalman filter

double yAngleKal, xAngleKal2;

uint32_t timer;
uint8_t i2cData[14]; // Buffer for I2C data

void setup() {
  Serial.begin(115200);
  Wire.begin();
  TWBR = ((F_CPU / 400000L) - 16) / 2; // Set I2C frequency to 400kHz

  i2cData[0] = 7; // Set the sample rate to 1000Hz - 8kHz/(7+1) = 1000Hz
  i2cData[1] = 0x00; // Disable FSYNC and set 260 Hz Acc filtering, 256 Hz Gyro filtering, 8 KHz sampling
  i2cData[2] = 0x00; // Set Gyro Full Scale Range to ±250deg/s
  i2cData[3] = 0x00; // Set Accelerometer Full Scale Range to ±2g
  while (i2cWrite(0x19, i2cData, 4, false)); // Write to all four registers at once
  while (i2cWrite(0x6B, 0x01, true)); // PLL with X axis gyroscope reference and disable sleep mode

  while (i2cRead(0x75, i2cData, 1));
  if (i2cData[0] != 0x68) { // Read "WHO_AM_I" register
    Serial.print(F("Error reading sensor"));
    while (1);
  }

  delay(100); // Wait for sensor to stabilize

  /* Set kalman and gyro starting angle */
  while (i2cRead(0x3B, i2cData, 6));
  accX = (i2cData[0] << 8) | i2cData[1];
  accY = (i2cData[2] << 8) | i2cData[3];
  accZ = (i2cData[4] << 8) | i2cData[5];
  // Source: http://www.freescale.com/files/sensors/doc/app_note/AN3461.pdf eq. 25 and eq. 26
  // atan2 outputs the value of -π to π (radians) - see http://en.wikipedia.org/wiki/Atan2
  // It is then converted from radians to degrees
  double yAngle2  = atan2(accY, accZ) * RAD_TO_DEG;
  double xAngle = atan(-accX / sqrt(accY * accY + accZ * accZ)) * RAD_TO_DEG;
  double yAngle  = atan(accY / sqrt(accX * accX + accZ * accZ)) * RAD_TO_DEG;
  double xAngle2 = atan2(-accX, accZ) * RAD_TO_DEG;


  kalmanY2.setAngle(yAngle2); // Set starting angle
  kalmanX.setAngle(xAngle);
  kalmanY.setAngle(yAngle); // Set starting angle
  kalmanX2.setAngle(xAngle2);

  timer = micros();
}

void loop() {
  while (i2cRead(0x3B, i2cData, 14));
  accX = ((i2cData[0] << 8) | i2cData[1]);
  accY = ((i2cData[2] << 8) | i2cData[3]);
  accZ = ((i2cData[4] << 8) | i2cData[5]);
  tempRaw = (i2cData[6] << 8) | i2cData[7];
  gyroX = (i2cData[8] << 8) | i2cData[9];
  gyroY = (i2cData[10] << 8) | i2cData[11];
  gyroZ = (i2cData[12] << 8) | i2cData[13];

  double dt = (double)(micros() - timer) / 1000000; // Calculate delta time
  timer = micros();

  // Source: http://www.freescale.com/files/sensors/doc/app_note/AN3461.pdf eq. 25 and eq. 26
  // atan2 outputs the value of -π to π (radians) - see http://en.wikipedia.org/wiki/Atan2
  // It is then converted from radians to degrees
  double yAngle2  = atan2(accY, accZ) * RAD_TO_DEG;
  double xAngle = atan(-accX / sqrt(accY * accY + accZ * accZ)) * RAD_TO_DEG;
  double yAngle  = atan(accY / sqrt(accX * accX + accZ * accZ)) * RAD_TO_DEG;
  double xAngle2 = atan2(-accX, accZ) * RAD_TO_DEG;


  double gyroXrate = gyroX / 131.0; // Convert to deg/s
  double gyroYrate = gyroY / 131.0; // Convert to deg/s



  // This fixes the transition problem when the accelerometer angle jumps between -180 and 180 degrees
  if ((yAngle2 < -90 && yAngleKal2 > 90) || (yAngle2 > 90 && yAngleKal2 < -90)) {
    kalmanY2.setAngle(yAngle2);
    yAngleKal2 = yAngle2;
  } else
    yAngleKal2 = kalmanY2.getAngle(yAngle2, gyroXrate, dt); // Calculate the angle using a Kalman filter

  if (abs(yAngleKal2) > 90)
    gyroYrate = -gyroYrate; // Invert rate, so it fits the restriced accelerometer reading
  xAngleKal = kalmanX.getAngle(xAngle, gyroYrate, dt);
  if ((xAngle2 < -90 && xAngleKal2 > 90) || (xAngle2 > 90 && xAngleKal2 < -90)) {
    kalmanX2.setAngle(xAngle2);
    xAngleKal2 = xAngle2;
  } else
    xAngleKal2 = kalmanX2.getAngle(xAngle2, gyroYrate, dt); // Calculate the angle using a Kalman filter

  if (abs(xAngleKal2) > 90)
    gyroXrate = -gyroXrate; // Invert rate, so it fits the restriced accelerometer reading
  yAngleKal = kalmanY.getAngle(yAngle, gyroXrate, dt); // Calculate the angle using a Kalman filter

  Serial.print(yAngle); Serial.print("\t");
  Serial.print(yAngleKal); Serial.print("\t");
  Serial.print(xAngle); Serial.print("\t");
  Serial.print(xAngleKal); Serial.print("\t");


  Serial.print("\r\n");
  delay(2);
}
