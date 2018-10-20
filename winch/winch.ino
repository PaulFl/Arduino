#include <VescUart.h>
#include <LiquidCrystal.h>

const float wheelDiameter = 0.085;
const int magnets = 42; //14 magnets * 3

int cellNumber = 4;

int redLEDPin = 6;
int greenLEDPin = 7;
int killSwitchPin = 8;
int buttonPin = 9;



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
  UART.setSerialPort(&Serial);
}

void loop() {
  if (UART.getVescValues()) {
    if (UART.data.inpVoltage > 18){
      cellNumber = 6;
    }
    if (UART.data.inpVoltage/cellNumber < 3.65){
      digitalWrite(redLEDPin, 1);
    } else {
      digitalWrite(redLEDPin, 0); 
    }
    lcd.setCursor(0, 0);
    lcd.print("IN: ");
    lcd.print(UART.data.inpVoltage/cellNumber);
    lcd.print("V (");
    lcd.print(cellNumber);
    lcd.print("S)       ");
    lcd.setCursor(0, 1);
    lcd.print("Length: ");
    lcd.print(UART.data.tachometer/magnets*PI * wheelDiameter);
    lcd.print("m       ");
  }
  delay(50);
}
