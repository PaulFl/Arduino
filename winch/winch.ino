#include <VescUart.h>
#include <LiquidCrystal.h>

#define BUTTONDEBOUNCE 50

const float wheelDiameter = 0.085;
const int magnets = 42; //14 magnets * 3

int cellNumber = 4;

int redLEDPin = 6;
int greenLEDPin = 7;
int killSwitchPin = 8;
int buttonPin = 9;
int potPin = A0;

int pageNumber = 0;
int pageMax = 3;
int launchCount = 0;

bool warningSignal = false;
bool killSwitchState = false;
bool lastButtonState = false;

long lastButton;
float potValue = 0;

float maxMotorCurrent = 0;
float maxBatteryCurrent = 0;



LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

VescUart UART;

/* Values:
    avgMotorCurrent
    avgInputCurrent
    dutyCycleNow
    rpm
    inpVoltage
    ampHours
    ampHoursCharged
    tachometer
    tachometerAbs
*/

byte gamma[] = {
  B10001,
  B10001,
  B01010,
  B00100,
  B01010,
  B10001,
  B10001,
  B01110
};
byte oneC[8] = {
  B10000,
  B10000,
  B10000,
  B10000,
  B10000,
  B10000,
  B10000,
  B10000
};
byte twoC[8] = {
  B11000,
  B11000,
  B11000,
  B11000,
  B11000,
  B11000,
  B11000,
  B11000
};
byte threeC[8] = {
  B11100,
  B11100,
  B11100,
  B11100,
  B11100,
  B11100,
  B11100,
  B11100
};
byte fourC[8] = {
  B11110,
  B11110,
  B11110,
  B11110,
  B11110,
  B11110,
  B11110,
  B11110
};
byte fiveC[] = {
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111
};



void setup() {
  Serial.begin(9600);
  pinMode(redLEDPin, OUTPUT);
  pinMode(greenLEDPin, OUTPUT);
  pinMode(killSwitchPin, INPUT_PULLUP);
  pinMode(buttonPin, INPUT_PULLUP);

  digitalWrite(greenLEDPin, 1);
  digitalWrite(redLEDPin, 1);


  lcd.begin(16, 2);
  lcd.createChar(0, gamma);
  lcd.createChar(1, oneC);
  lcd.createChar(2, twoC);
  lcd.createChar(3, threeC);
  lcd.createChar(4, fourC);
  lcd.createChar(5, fiveC);
  lcd.setCursor(0, 0);
  lcd.print(" Winch Monitor  ");
  lcd.setCursor(0, 1);
  lcd.print("Paul Fleury ");
  for (int i = 1; i <= 4; i++) {
    for (int j = 1; j <= 5; j++) {
      delay(3000 / 20);
      lcd.setCursor(11 + i, 1);
      lcd.write(byte(j));
    }
  }

  digitalWrite(greenLEDPin, 0);
  digitalWrite(redLEDPin, 0);


  UART.setSerialPort(&Serial);

  lastButton = millis();
}

void loop() {
  killSwitchState = !digitalRead(killSwitchPin);
  checkButton();

  if (UART.getVescValues()) {
    if (UART.data.inpVoltage > 18) {
      cellNumber = 6;
    }
    if (UART.data.avgMotorCurrent > maxMotorCurrent){
      maxMotorCurrent = UART.data.avgMotorCurrent;
    }
    if (UART.data.avgInputCurrent > maxBatteryCurrent){
      maxBatteryCurrent = UART.data.avgInputCurrent;
    }
    warningSignal = (UART.data.inpVoltage / cellNumber < 3.65);

    displayData();

  }
  readPot();


  if (!killSwitchState) {
    digitalWrite(greenLEDPin, 0);
    UART.setCurrent(0);
  } else {
    digitalWrite(greenLEDPin, 1);
    UART.setDuty(potValue);
  }
  digitalWrite(redLEDPin, warningSignal);
}

void displayData() {
  switch (pageNumber) {
    case 0:
      lcd.setCursor(0, 0);
      lcd.print("IN: ");
      lcd.print(UART.data.inpVoltage / cellNumber);
      lcd.print("V (");
      lcd.print(cellNumber);
      lcd.print("S)       ");
      lcd.setCursor(0, 1);
      lcd.print("Launches: ");
      lcd.print(launchCount);
      lcd.print("       ");
      break;
    case 1:
      lcd.setCursor(0, 0);
      lcd.print("Length: ");
      lcd.print(UART.data.tachometer / magnets * PI * wheelDiameter);
      lcd.print("m       ");
      lcd.setCursor(0, 1);
      lcd.print("Input: ");
      lcd.print(int(potValue*100));
      lcd.print("%      ");
      break;
    case 2:
      lcd.setCursor(0, 0);
      lcd.print("Current M: ");
      lcd.print(UART.data.avgMotorCurrent);
      lcd.print("A     ");
      lcd.setCursor(0, 1);
      lcd.print("Current B: ");
      lcd.print(UART.data.avgInputCurrent);
      lcd.print("A      ");
      break;
      case 3:
      lcd.setCursor(0, 0);
      lcd.print("Motor max: ");
      lcd.print(maxMotorCurrent);
      lcd.print("A     ");
      lcd.setCursor(0, 1);
      lcd.print("Batt max:  ");
      lcd.print(maxBatteryCurrent);
      lcd.print("A      ");

    default:
      break;
  }
}

void checkButton() {
  if (!digitalRead(buttonPin) && millis() - lastButton > BUTTONDEBOUNCE && !lastButtonState) {
    lastButtonState = true;
    buttonPressed();
  } else if (digitalRead(buttonPin)) {
    lastButtonState = false;
  }
}

void buttonPressed() {
  pageNumber ++;
  if (pageNumber > pageMax) {
    pageNumber = 0;
  }
}

void readPot() {
  potValue = float(analogRead(potPin))/float(1023);
}
