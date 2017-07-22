/*!!!!!!!!! in IRremonteInt.h, set

  // Arduino Duemilanove, Diecimila, LilyPad, Mini, Fio, Nano, etc
  #else
  //#define IR_USE_TIMER1   // tx = pin 9
  #define IR_USE_TIMER2     // tx = pin 3

  #endif */



#include <IRremote.h>
#include <IRremoteInt.h>

#define DEBUG 1
#define HIGHTHRESHOLD 6600
#define LOWTHRESHOLD 6300
#define SPEED 100
#define LEDPAUSE 5000
#define IRDELAY 100


#include "irValues.h"
#include <CapacitiveSensor.h>


short sendPin = A3;
short receivePin = A0;

short relay1 = 2;
short relay2 = 4;
short red = 9;
short green = 10;
short blue = 6;
short iRReceiver = 7;

int newBlue = 0;
int newRed = 0;
int newGreen = 0;

int blueValue = 0;
int greenValue = 0;
int redValue = 0;

bool colorReached = true;

unsigned long lastChange;
unsigned long lastColor;
unsigned long pastIRReading;

int dimColor = 10;


int color;

boolean irClean = false;

int irMainValue;

bool relay1State = LOW;
bool relay2State = LOW;
bool touched = false;
bool playColor = true;
bool ledPower = true;

CapacitiveSensor capSensor = CapacitiveSensor(sendPin, receivePin);
long sensorValue;


IRrecv irrecv(iRReceiver);
decode_results results;

void setup() {
  if (DEBUG) Serial.begin(9600);

  pinMode(relay1, OUTPUT);
  digitalWrite(relay1, LOW);

  pinMode(relay2, OUTPUT);
  digitalWrite(relay2, LOW);

  pinMode(red, OUTPUT);
  pinMode(blue, OUTPUT);
  pinMode(green, OUTPUT);

  lastChange = millis();
  lastColor = millis();
  pastIRReading = millis();

  irrecv.enableIRIn();
}

void loop() {
  irMainValue = readIR();
  switch (irMainValue) {
    case PLAYPAUSE:
      playColor = !playColor;
      break;
    case PREVIOUS:
      playColor = false;
      color--;
      if (color == -1) color = 5;
      setColor();
      break;
    case NEXT:
      playColor = false;
      color++;
      if (color == 6) color = 0;
      setColor();
      break;
    case PLUS:
      dimColor ++;
      if (dimColor == 11) dimColor = 10;
      break;
    case MINUS:
      dimColor --;
      if (dimColor == 0) dimColor = 1;
      break;
    case GREEN:
      relay2State = !relay2State;
      if (DEBUG) Serial.println("Switched");
      digitalWrite(relay2, relay2State);
      break;
    case RED:
      relay1State = !relay1State;
      if (DEBUG) Serial.println("Switched");
      digitalWrite(relay1, relay1State);
      break;
    case POWER:
      ledPower = !ledPower;
      if (ledPower) {
        analogWrite(blue, blueValue);
        analogWrite(green, greenValue);
        analogWrite(red, redValue);
      } else {
        digitalWrite(blue, LOW);
        digitalWrite(red, LOW);
        digitalWrite(green, LOW);
      }
      break;
    default:
      break;
  }

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

int readIR() {
  if (millis() - pastIRReading > IRDELAY && !irClean) {
    if (irrecv.decode(&results)) {
      irrecv.resume();
    }
    irClean = true;
  }
  if (irClean) {
    if (irrecv.decode(&results)) {
      irrecv.resume();
      irClean = false;
      pastIRReading = millis();
      String value = String(results.value, HEX);
      value.remove(0, 4);
      value.remove(0, 1);
      if (value == left[0] || value == left[1] || value == left[2]) return LEFT;
      else if (value == right[0] || value == right[1] || value == right[2]) return RIGHT;
      else if (value == up[0] || value == up[1] || value == up[2]) return UP;
      else if (value == down[0] || value == down[1] || value == down[2]) return DOWN;
      else if (value == enter[0] || value == enter[1] || value == enter[2]) return ENTER;
      else if (value == settings[0] || value == settings[1] || value == settings[2]) return SETTINGS;
      else if (value == list[0] || value == list[1] || value == list[2]) return LIST;
      else if (value == zero[0] || value == zero[1] || value == zero[2]) return 0;
      else if (value == one[0] || value == one[1] || value == one[2]) return 1;
      else if (value == two[0] || value == two[1] || value == two[2]) return 2;
      else if (value == three[0] || value == three[1] || value == three[2]) return 3;
      else if (value == four[0] || value == four[1] || value == four[2]) return 4;
      else if (value == five[0] || value == five[1] || value == five[2]) return 5;
      else if (value == six[0] || value == six[1] || value == six[2]) return 6;
      else if (value == seven[0] || value == seven[1] || value == seven[2]) return 7;
      else if (value == eight[0] || value == eight[1] || value == eight[2]) return 8;
      else if (value == nine[0] || value == nine[1] || value == nine[2]) return 9;
      else if (value == plus[0] || value == plus[1] || value == plus[2]) return PLUS;
      else if (value == minus[0] || value == minus[1] || value == minus[2]) return MINUS;
      else if (value == playpause[0] || value == playpause[1] || value == playpause[2]) return PLAYPAUSE;
      else if (value == redButton[0] || value == redButton[1] || value == redButton[2]) return RED;
      else if (value == greenButton[0] || value == greenButton[1] || value == greenButton[2]) return GREEN;
      else if (value == previous[0] || value == previous[1] || value == previous[2]) return PREVIOUS;
      else if (value == next[0] || value == next[1] || value == next[2]) return NEXT;
      else if (value == power[0] || value == power[1] || value == power[2]) return POWER;
    }
  }
  return 999;
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

