#include <nRF24L01.h>
#include <printf.h>
#include <RF24.h>
#include <RF24_config.h>

#include <Servo.h>


//Settings
#define DEADZONECRUISE 10
#define SENSIBILITYDIV 35
#define ACCELERATIONDELAY 400
#define BEGINNERDIV 2
#define MOTORIDLE 90
#define TIMEOUTDELAY 500
#define COLORCHANGESPEED 3
#define STILLCOLOR 2000
#define STILLCOLORCRAZY 300
#define BUTTONDELAY 400

//Pins
short RF24cePin = 7;
short RF24csnPin = 8;

short motorPin = 9;

short redPin = 3;
short greenPin = 5;
short bluePin = 6;

#define MOTOR 0
#define HORIZONTAL 1
#define CBUTTON 3
#define ZBUTTON 2

//Leds
int stillColor = STILLCOLOR;
int colorChangeSpeed = COLORCHANGESPEED;

int newBlue = 0;
int newRed = 0;
int newGreen = 0;

int blueValue = 0;
int greenValue = 0;
int redValue = 0;

short color = 0;

//Initialisations

bool hold = false;
bool discoMode = false;
bool crazyColor = false;
bool beginnerMode = false;
bool brakeAllowed = true;

bool lastCButtonState = false;
bool lastZButtonState = false;
int lastHorizontalValue = 50;

long lastRead;
long lastColorChange;
long lastColorDisco;
long lastAcceleration;

int msg[4];

Servo motor;
RF24 radio(RF24cePin, RF24csnPin);
const uint64_t pipe = 0xE8E8F0F0E1LL;//White nunchuck
//const uint64_t pipe = 0xE8E8F0F1E0LL;//Aymeric


void setColor() {
  switch (color) {
    case 0:
      newBlue = 255;
      newGreen = 0;
      newRed = 0;
      break;
    case 1:
      newBlue = 0;
      newGreen = 255;
      newRed = 0;
      break;
    case 2:
      newBlue = 0;
      newGreen = 0;
      newRed = 255;
      break;
    case 3:
      newBlue = 255;
      newGreen = 255;
      newRed = 0;
      break;
    case 4:
      newBlue = 0;
      newGreen = 255;
      newRed = 255;
      break;
    case 5:
      newBlue = 255;
      newGreen = 0;
      newRed = 255;
      break;
    case 6:
      newBlue = 255;
      newGreen = 255;
      newRed = 255;
      break;
    case 7:
      newBlue = 0;
      newGreen = 0;
      newRed = 0;
      break;
  }
}

void refreshLed() {
  if (millis() - lastColorChange > colorChangeSpeed) {
    lastColorChange = millis();
    if (blueValue > newBlue) {
      blueValue--;
    } else if ( blueValue < newBlue) {
      blueValue++;
    }
    if (greenValue > newGreen) {
      greenValue--;
    } else if (greenValue < newGreen) {
      greenValue++;
    }
    if (redValue > newRed) {
      redValue--;
    } else if (redValue < newRed) {
      redValue++;
    }
  }

  if (discoMode && (millis() - lastColorDisco) > stillColor) {
    lastColorDisco = millis();
    if (color >= 6) color = 0;
    else color++;
    setColor();
  }

  analogWrite(bluePin, blueValue);
  analogWrite(greenPin, greenValue);
  analogWrite(redPin, redValue);
}

void ledButtonPressed() {
  if (discoMode && !crazyColor) { //Turn led off
    discoMode = false;
    color = 7;
    setColor();
    redValue = 0;
    blueValue = 0;
    greenValue = 0;
    analogWrite(bluePin, blueValue);
    analogWrite(greenPin, greenValue);
    analogWrite(redPin, redValue);
  } else {
    discoMode = true;
    crazyColor = !crazyColor;
    if (crazyColor) {
      stillColor = STILLCOLORCRAZY;
      colorChangeSpeed = 0;
    } else {
      stillColor = STILLCOLOR;
      colorChangeSpeed = COLORCHANGESPEED;
    }
  }
}

void sendMotor(int value) {
  if (value >= 89 && !beginnerMode) motor.write(value);
  else if (value >= 89 && value <= (90 + 90 / BEGINNERDIV)) motor.write(value);
  else if (value >= (90 + 90 / BEGINNERDIV)) motor.write((90 + 90 / BEGINNERDIV));
  else if (value < 89 && brakeAllowed) motor.write(value);
  else motor.write(MOTORIDLE);
}

void getData() {
  if (millis() - lastRead > TIMEOUTDELAY) { // Let go if signal timeout
    sendMotor(MOTORIDLE);
    brakeAllowed = false;
  }
  if (radio.available()) {
    while (radio.available()) radio.read(msg, 7); //Get data
    if (msg[MOTOR] >= 89) brakeAllowed = true;
    if (!msg[CBUTTON] && lastCButtonState && !msg[ZBUTTON]) ledButtonPressed();
    if (!msg[ZBUTTON] && lastZButtonState) Serial.write(2);
    if (msg[ZBUTTON] && !beginnerMode) sendMotor(msg[MOTOR]);
    else if (msg[ZBUTTON] && beginnerMode) {
      if (msg[MOTOR] >= 89) sendMotor(map(msg[MOTOR], 0, 180, (90 -30 -60 / BEGINNERDIV), (90 +30+ 60 / BEGINNERDIV)));
      else sendMotor(msg[MOTOR]);
    }
    else if (!msg[ZBUTTON] && motor.read() >= 89) { //If zbutton and not braking
      if (msg[MOTOR] < (180 - DEADZONECRUISE) / 2 && (motor.read() - ((180 - DEADZONECRUISE) / 2 - msg[MOTOR]) / SENSIBILITYDIV) >= 89 && lastAcceleration - millis() > ACCELERATIONDELAY) { //if doesnt not imply brakes
        sendMotor(motor.read() - ((180 - DEADZONECRUISE) / 2 - msg[MOTOR]) / SENSIBILITYDIV); //reduce speed
        brakeAllowed = false;
        lastAcceleration = millis();
      } else if (msg[MOTOR] <= (180 + DEADZONECRUISE) / 2 && lastAcceleration - millis() > ACCELERATIONDELAY) { //If in deadzone
        sendMotor(motor.read());
        lastAcceleration = millis();
        if (msg[HORIZONTAL] >= 95) beginnerMode = true;
        else if (msg[HORIZONTAL] <= 5) beginnerMode = false;
      } else if (lastAcceleration - millis() > ACCELERATIONDELAY) {
        sendMotor(motor.read() + (msg[MOTOR] - (180 + DEADZONECRUISE) / 2) / SENSIBILITYDIV);
        lastAcceleration = millis();
        brakeAllowed = true;
      } else {
        sendMotor(motor.read());
      }
    }
    if (msg[ZBUTTON] && msg[HORIZONTAL] >= 80 && lastHorizontalValue < 95) Serial.write(2);
    else if (msg[ZBUTTON] && msg[HORIZONTAL] <= 20 && lastHorizontalValue > 5) Serial.write(1);

    lastZButtonState = msg[ZBUTTON];
    lastCButtonState = msg[CBUTTON];
    lastHorizontalValue = msg[HORIZONTAL];
    lastRead = millis();
  }
}

void setup() {
  Serial.begin(9600);
  motor.attach(motorPin);
  sendMotor(MOTORIDLE);

  radio.begin();
  radio.openReadingPipe(1, pipe);
  radio.startListening();

  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  lastRead = millis();
  lastColorChange = millis();
  lastColorDisco = millis();
  lastAcceleration = millis();
}

void loop() {
  getData();
  refreshLed();

}
