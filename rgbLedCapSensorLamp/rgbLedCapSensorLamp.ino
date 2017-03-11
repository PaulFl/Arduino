/*!!!!!!!!! in IRremonteInt.h, set

  // Arduino Duemilanove, Diecimila, LilyPad, Mini, Fio, Nano, etc
  #else
  //#define IR_USE_TIMER1   // tx = pin 9
  #define IR_USE_TIMER2     // tx = pin 3

  #endif */

#define DEBUG 1
#define HIGHTHRESHOLD 6100
#define LOWTHRESHOLD 5800
#define SPEED 100
#define LEDPAUSE 5000


#include <CapacitiveSensor.h>


short sendPin = 4;
short receivePin = 7;

short relay1 = A5;
short red = 3;
short green = 9;
short blue = 10;

int newBlue = 0;
int newRed = 0;
int newGreen = 0;

int blueValue = 0;
int greenValue = 0;
int redValue = 0;

bool colorReached = false;

unsigned long lastChange;
unsigned long lastColor;



int color;



bool relay1State = LOW;
bool touched = false;
bool playColor = true;
bool ledPower = true;

CapacitiveSensor capSensor = CapacitiveSensor(sendPin, receivePin);
long sensorValue;



void setup() {
  if (DEBUG) Serial.begin(9600);

  pinMode(relay1, OUTPUT);
  digitalWrite(relay1, LOW);


  pinMode(red, OUTPUT);
  pinMode(blue, OUTPUT);
  pinMode(green, OUTPUT);

  lastChange = millis();
  lastColor = millis();

}

void loop() {


  sensorValue = capSensor.capacitiveSensorRaw(100);

  if (DEBUG) Serial.println(sensorValue);

  if (sensorValue > HIGHTHRESHOLD && !touched) {
    touched = true;
    digitalWrite(red, LOW);
    digitalWrite(blue, LOW);
    digitalWrite(green, LOW);
    relay1State = !relay1State;
    if (DEBUG) Serial.println("Switched");
    digitalWrite(relay1, relay1State);
  } else if (sensorValue < LOWTHRESHOLD && touched) {
    touched = false;
    if (ledPower) {
      analogWrite(blue, blueValue);
      analogWrite(green, greenValue);
      analogWrite(red, redValue);
    }
  }

  if (blueValue == newBlue && redValue == newRed && greenValue == newGreen && !colorReached) {
    lastColor = millis();
    colorReached = true;
  } else if (colorReached && millis() - lastColor > LEDPAUSE && playColor) {
    colorReached = false;
    color = random(6);
    setColor();
  } if (millis() - lastChange > SPEED) {
    lastChange = millis();
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
    } if (!touched && ledPower) {
      analogWrite(blue, blueValue);
      analogWrite(green, greenValue);
      analogWrite(red, redValue);
    }
  }

}



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
  }
}

