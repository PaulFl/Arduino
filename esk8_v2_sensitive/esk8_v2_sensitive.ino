#include <nRF24L01.h>
#include <printf.h>
#include <RF24.h>
#include <RF24_config.h>

#include <Servo.h>

#define DEBUG 1

//Settings
#define MAXBEGINNERSPEED 95
#define MOTORIDLE 90
#define TIMEOUTDELAY 400
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
bool beginnersMode = false;

bool lastCButtonState = false;
bool lastZButtonState = false;
int lastHorizontalValue = 50;

long lastRead;
long lastColorChange;
long lastColorDisco;

int msg[4];

Servo motor;
RF24 radio(RF24cePin, RF24csnPin);
const uint64_t pipe = 0xE8E8F0F0E1LL;

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

void getData() {
  if (millis() - lastRead > TIMEOUTDELAY) { // Let go if signal timeout
    motor.write(MOTORIDLE);
    if (DEBUG) {
      Serial.println("FAULT: Timeout");
    }
  }
  if (radio.available()) {
    while (radio.available()) radio.read(msg, 8); //Get data
    if (!msg[CBUTTON] && lastCButtonState) ledButtonPressed();
    if (msg[ZBUTTON]) motor.write(msg[MOTOR]);
    else if (!msg[ZBUTTON]) motor.write(motor.read());


    lastZButtonState = msg[ZBUTTON];
    lastCButtonState = msg[CBUTTON];
    lastHorizontalValue = msg[HORIZONTAL];
    lastRead = millis();
  }
}

void setup() {
  if (DEBUG) {
    Serial.begin(9600);
  }
  motor.attach(motorPin);
  motor.write(MOTORIDLE);

  radio.begin();
  radio.openReadingPipe(1, pipe);
  radio.startListening();

  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  lastRead = millis();
  lastColorChange = millis();
  lastColorDisco = millis();
}

void loop() {
  getData();
  refreshLed();
  if (DEBUG) {
    Serial.println(motor.read());
  }


}
