#include <Wire.h>
#include "Kalman.h" // Source: https://github.com/TKJElectronics/KalmanFilter

struct COORD {
  int acOffset;
  int gyOffset;
  double ac;
  int gy;
  double angle;
  double angle2;
  long int acLowPass = 0;
  int target;
  int speed;
  int error = 0;
  int sumError = 0;
  Kalman kalman;
  Kalman kalman2;
  double angleKal;
  double angleKal2;
  double gyroRate;
};

COORD x;
COORD y;
COORD z;

/* IMU Data */
int16_t Tmp;

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
  x.ac = (i2cData[0] << 8) | i2cData[1];
  y.ac = (i2cData[2] << 8) | i2cData[3];
  z.ac = (i2cData[4] << 8) | i2cData[5];
  y.angle2  = atan2(y.ac, z.ac) * RAD_TO_DEG;
  x.angle = atan(-x.ac / sqrt(y.ac * y.ac + z.ac * z.ac)) * RAD_TO_DEG;
  y.angle  = atan(y.ac / sqrt(x.ac * x.ac + z.ac * z.ac)) * RAD_TO_DEG;
  x.angle2 = atan2(-x.ac, z.ac) * RAD_TO_DEG;


  y.kalman2.setAngle(y.angle2); // Set starting angle
  x.kalman.setAngle(x.angle);
  y.kalman.setAngle(y.angle); // Set starting angle
  x.kalman2.setAngle(x.angle2);

  timer = micros();
}

void loop() {
  while (i2cRead(0x3B, i2cData, 14));
  x.ac = ((i2cData[0] << 8) | i2cData[1]);
  y.ac = ((i2cData[2] << 8) | i2cData[3]);
  z.ac = ((i2cData[4] << 8) | i2cData[5]);
  Tmp = (i2cData[6] << 8) | i2cData[7];
  y.gy = (i2cData[8] << 8) | i2cData[9];
  x.gy = (i2cData[10] << 8) | i2cData[11];
  z.gy = (i2cData[12] << 8) | i2cData[13];

  double dt = (double)(micros() - timer) / 1000000; // Calculate delta time
  timer = micros();

  y.angle2  = atan2(y.ac, z.ac) * RAD_TO_DEG;
  x.angle = atan(-x.ac / sqrt(y.ac * y.ac + z.ac * z.ac)) * RAD_TO_DEG;
  y.angle  = atan(y.ac / sqrt(x.ac * x.ac + z.ac * z.ac)) * RAD_TO_DEG;
  x.angle2 = atan2(-x.ac, z.ac) * RAD_TO_DEG;


  y.gyroRate = y.gy / 131.0; // Convert to deg/s
  x.gyroRate = x.gy / 131.0; // Convert to deg/s



  // This fixes the transition problem when the accelerometer angle jumps between -180 and 180 degrees
  if ((y.angle2 < -90 && y.angleKal2 > 90) || (y.angle2 > 90 && y.angleKal2 < -90)) {
    y.kalman2.setAngle(y.angle2);
    y.angleKal2 = y.angle2;
  } else
    y.angleKal2 = y.kalman2.getAngle(y.angle2, y.gyroRate, dt); // Calculate the angle using a Kalman filter

  if (abs(y.angleKal2) > 90)
    x.gyroRate = -x.gyroRate; // Invert rate, so it fits the restriced accelerometer reading
  x.angleKal = x.kalman.getAngle(x.angle, x.gyroRate, dt);
  if ((x.angle2 < -90 && x.angleKal2 > 90) || (x.angle2 > 90 && x.angleKal2 < -90)) {
    x.kalman2.setAngle(x.angle2);
    x.angleKal2 = x.angle2;
  } else
    x.angleKal2 = x.kalman2.getAngle(x.angle2, x.gyroRate, dt); // Calculate the angle using a Kalman filter

  if (abs(x.angleKal2) > 90)
    y.gyroRate = -y.gyroRate; // Invert rate, so it fits the restriced accelerometer reading
  y.angleKal = y.kalman.getAngle(y.angle, y.gyroRate, dt); // Calculate the angle using a Kalman filter

  Serial.print(y.angle); Serial.print("\t");
  Serial.print(y.angleKal); Serial.print("\t");
  Serial.print(x.angle); Serial.print("\t");
  Serial.print(x.angleKal); Serial.print("\t");


  Serial.print("\r\n");
  delay(2);
}
