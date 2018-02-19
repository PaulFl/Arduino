#include <CapacitiveSensor.h>

#define DEBUG 0
#define HIGHTHRESHOLD 6600
#define LOWTHRESHOLD 6300
#define SPEED 100
#define LEDPAUSE 5000

String serialIn = "";


short sendPin = A3;
short receivePin = A0;

short relay1 = 2;
short relay2 = 4;
short v12 = A5;
short red = 9;
short green = 10;
short blue = 6;

int newBlue = 0;
int newRed = 0;
int newGreen = 0;

int blueValue = 0;
int greenValue = 0;
int redValue = 0;

bool colorReached = true;

unsigned long lastChange;
unsigned long lastColor;

int dimColor = 10;


int color;



bool relay1State = LOW;
bool relay2State = LOW;
bool v12State = LOW;
bool touched = false;
bool playColor = true;
bool ledPower = true;

CapacitiveSensor capSensor = CapacitiveSensor(sendPin, receivePin);
long sensorValue;

void setup() {
  Serial.begin(9600);

  pinMode(relay1, OUTPUT);
  digitalWrite(relay1, !LOW);

  pinMode(relay2, OUTPUT);
  digitalWrite(relay2, LOW);

  pinMode(v12, OUTPUT);
  digitalWrite(v12, LOW);

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
    digitalWrite(relay1, !relay1State);
    sendChangeSerial(1, relay1State);
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
    } else {
      analogWrite(blue, 0);
      analogWrite(red, 0);
      analogWrite(green, 0);
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

void sendChangeSerial(int id, bool newState) {
  Serial.print(id);
  Serial.println(int(newState));
}

void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    serialIn += inChar;
    if (inChar == '\n') {
      int id = serialIn.substring(0, 1).toInt();
      bool state = serialIn.substring(1).toInt();
      switch (id) {
        case 1:
          relay1State = state;
          digitalWrite(relay1, !relay1State);
          sendChangeSerial(1, relay1State);
          break;
        case 2:
          relay2State = state;
          digitalWrite(relay2, relay2State);
          sendChangeSerial(2, relay2State);
          break;
        case 3:
          ledPower = state;
          sendChangeSerial(3, state);
          break;
        case 4:
          v12State = state;
          digitalWrite(v12, v12State);
          sendChangeSerial(4, v12State);
          break;
        default:
          break;
      }
      serialIn = "";
    }
  }
}
