#include <nRF24L01.h>
#include <printf.h>
#include <RF24.h>
#include <RF24_config.h>

#include <Servo.h>

#define ACCELERATIONDELAY 40
#define ACCELERATIONDELAYBEGINNER 60
#define MAXBEGINNERSPEED 95
#define MOTORIDLE 90
#define TIMEOUTDELAY 400
#define COLORCHANGESPEED 3
#define STILLCOLOR 2000
#define STILLCOLORCRAZY 300
#define BUTTONDELAY 400

#define MOTOR 0
#define HORIZONTAL 1
#define CBUTTON 2
#define ZBUTTON 3

int stillColor = STILLCOLOR;
int colorChangeSpeed = COLORCHANGESPEED;

short RF24cePin = 7;
short RF24csnPin = 8;

short motorPin = 9;

short redPin = 3;
short greenPin = 5;
short bluePin = 6;

int newBlue = 0;
int newRed = 0;
int newGreen = 0;

int blueValue = 0;
int greenValue = 0;
int redValue = 0;

short color = 0;

int acceleration = ACCELERATIONDELAY;

int holdSpeed;
bool hold = false;
bool holdRenewed = true;
bool discoMode = false;
bool crazyColor = false;
bool beginnersMode = false;

bool lastCButtonState = false;
bool lastZButtonState = false;
int lastHorizontalValue;

long lastAcceleration;
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

void getData() {
  if (millis() - lastRead > TIMEOUTDELAY) {
    motor.write(MOTORIDLE);
    hold = false;
    holdRenewed = true;
  }
  if (radio.available()) {
    while (radio.available()) radio.read(msg, 4);
    if (msg[ZBUTTON]) {
      motor.write(motor.read());
    } else {
      motor.write(msg[MOTOR]);
    }
    if (!msg[ZBUTTON] && msg[CBUTTON] && !lastCButtonState) {
      lastCButtonState = true;
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
    
  }
}

void setup() {
  motor.attach(motorPin);
  motor.write(MOTORIDLE);

  radio.begin();
  radio.openReadingPipe(1, pipe);
  radio.startListening();

  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  lastAcceleration = millis();
  lastRead = millis();
  lastColorChange = millis();
  lastColorDisco = millis();
}

void loop() {
  getData();
  refreshLed();

}
