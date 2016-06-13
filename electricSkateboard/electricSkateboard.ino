#include <nRF24L01.h>
#include <printf.h>
#include <RF24.h>
#include <RF24_config.h>

#include <Servo.h>

#define MOTORMIN 20
#define ACCELERATIONDELAY 30
#define BLINKPATTERNLENGTH 750
#define TIMEOUTDELAY 400
#define DELAYSOFTBRAKE 200
#define DELAYHARDBRAKE 1000
#define STARTSPEED 30

float decelerationFactor = 0.02;
float voltageRef = 4.67;
float lowBatteryPercentage = 0.09;
float highBatteryPercentage = 0.22;

short RF24cePin = 7;
short RF24csnPin = 8;

short powerLedPin = A4;
short batteryLedPin = A3;

short firstBrakePin = 2;
short secondBrakePin = A0;

short motorPin = 9;
short lipoCellPin = A6;

short USBLedPin = A2;
short USBHubPin = A5;

float voltage;
float batteryPercentage;
int motorSpeed;
int holdSpeed;
int restart;
int valueBeforeStop;
bool softBrake = false;
bool hardBrake = false;
bool lowBattery = false;
bool hold = false;
bool holdRenewed = true;

long startBraking;
long lipoBlinkMillis;
long lastAcceleration;
long lastRead;
long lastStop;

int msg[1];

Servo motor;
RF24 radio(RF24cePin, RF24csnPin);

const uint64_t pipe = 0xE8E8F0F0E1LL;

void setup() {
  Serial.begin(9600);
  motor.attach(motorPin);
  motor.write(MOTORMIN);

  radio.begin();
  radio.openReadingPipe(1, pipe);
  radio.startListening();

  pinMode(powerLedPin, OUTPUT);
  digitalWrite(powerLedPin, HIGH);
  pinMode(firstBrakePin, OUTPUT);
  pinMode(secondBrakePin, OUTPUT);
  pinMode(batteryLedPin, OUTPUT);
  digitalWrite(batteryLedPin, HIGH);
  pinMode(USBLedPin, OUTPUT);
  digitalWrite(USBLedPin, HIGH);
  pinMode(USBHubPin, OUTPUT);
  digitalWrite(USBHubPin, LOW);

  startBraking = millis();
  lipoBlinkMillis = millis();
  lastAcceleration = millis();
  lastRead = millis();
  lastStop = millis();

  digitalWrite(firstBrakePin, HIGH);
  digitalWrite(secondBrakePin, HIGH);
  delay(6000);
  digitalWrite(firstBrakePin, LOW);
  digitalWrite(secondBrakePin, LOW);
}

void loop() {
  //Get data
  if (millis() - lastRead > TIMEOUTDELAY) {
    motor.write(MOTORMIN);
    motorSpeed = MOTORMIN;
    lastStop = millis();
    hold = false;
    holdRenewed = true;
  }
  if (radio.available()) {
    digitalWrite(powerLedPin, HIGH);
    while (radio.available()) radio.read(msg, 2);
    if (msg[0] == 200) {
      if (holdRenewed) {
        hold = !hold;
        holdRenewed = false;
        holdSpeed = motor.read();
      }
      lastRead = millis();
    } else hardBrake = false;
    if (msg[0] <= 180 && msg[0] >= MOTORMIN) {
      hold = false;
      holdRenewed = true;
      lastRead = millis();
      motorSpeed = map(msg[0], MOTORMIN, 180, STARTSPEED, 180);
      restart = motor.read() - ((millis() - lastStop) * decelerationFactor);
      if (restart >= MOTORMIN) motor.write(restart);
    } else if (msg[0] == 0) {
      lastRead = millis();
      holdRenewed = true;
      if (hold) {
        motor.write(holdSpeed);
      } else {
        motor.write(MOTORMIN);
        motorSpeed = MOTORMIN;
        lastStop = millis();
      }
    }
  } else digitalWrite(powerLedPin, LOW);

  if (!softBrake && !hardBrake /*&& !lowBattery*/) {
    if (motorSpeed < motor.read()) motor.write(motorSpeed);
    else if (motorSpeed > motor.read() && (millis() - lastAcceleration) > ACCELERATIONDELAY) {
      motor.write(motor.read() + 1);
      lastAcceleration = millis();
    }
  }
  else motor.write(MOTORMIN);

  //Brakes

  if (hardBrake) {
    motor.write(MOTORMIN);
    motorSpeed = MOTORMIN;
    if (millis() - startBraking > DELAYHARDBRAKE) {
      digitalWrite(firstBrakePin, HIGH);
      digitalWrite(secondBrakePin, HIGH);
    } else if (millis() - startBraking > DELAYSOFTBRAKE) {
      digitalWrite(firstBrakePin, HIGH);
      digitalWrite(secondBrakePin, LOW);
    } else {
      digitalWrite(firstBrakePin, LOW);
      digitalWrite(secondBrakePin, LOW);
    }
  } else if (softBrake) {
    motor.write(MOTORMIN);
    motorSpeed = MOTORMIN;
    if (millis() - startBraking > DELAYSOFTBRAKE) {
      digitalWrite(firstBrakePin, HIGH);
      digitalWrite(secondBrakePin, LOW);
    } else {
      digitalWrite(firstBrakePin, LOW);
      digitalWrite(secondBrakePin, LOW);
    }
  } else {
    digitalWrite(firstBrakePin, LOW);
    digitalWrite(secondBrakePin, LOW);
    startBraking = millis();
  }

  //Battery percentage
  voltage = analogRead(lipoCellPin) * (voltageRef / 1023.0);
  if (voltage < 3.50) batteryPercentage = 0.03;
  else if (voltage < 3.55) batteryPercentage = 0.05;
  else if (voltage < 3.60) batteryPercentage = 0.07;
  else if (voltage < 3.65) batteryPercentage = 0.09;
  else if (voltage < 3.70) batteryPercentage = 0.15;
  else if (voltage < 3.75) batteryPercentage = 0.22;
  else if (voltage < 3.80) batteryPercentage = 0.34;
  else if (voltage < 3.85) batteryPercentage = 0.50;
  else if (voltage < 3.90) batteryPercentage = 0.60;
  else if (voltage < 3.95) batteryPercentage = 0.69;
  else if (voltage < 4.00) batteryPercentage = 0.77;
  else if (voltage < 4.05) batteryPercentage = 0.83;
  else if (voltage < 4.10) batteryPercentage = 0.89;
  else if (voltage < 4.15) batteryPercentage = 0.96;
  else batteryPercentage = 1;

  if (batteryPercentage <= lowBatteryPercentage) {
    lowBattery = true;
    digitalWrite(USBLedPin, LOW);
    digitalWrite(USBHubPin, HIGH);
  } else if (batteryPercentage >= highBatteryPercentage) {
    lowBattery = false;
    digitalWrite(USBLedPin, HIGH);
    digitalWrite(USBHubPin, LOW);
  }

  if ((millis() - lipoBlinkMillis) > (BLINKPATTERNLENGTH * batteryPercentage)) {
    digitalWrite(batteryLedPin, LOW);
  }
  if (millis() - lipoBlinkMillis >= BLINKPATTERNLENGTH) {
    lipoBlinkMillis = millis();
    digitalWrite(batteryLedPin, HIGH);
  }
}
