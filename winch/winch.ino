#include <VescUart.h>
#include <LiquidCrystal.h>



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
  lcd.begin(16, 2);
  lcd.createChar(0, gamma);
  lcd.createChar(1, oneC);
  lcd.createChar(2, twoC);
  lcd.createChar(3, threeC);
  lcd.createChar(4, fourC);
  lcd.createChar(5, fiveC);
  lcd.setCursor(0, 0);
  lcd.print("   ");
  lcd.write(byte(0));
  lcd.print(" Vanguard   ");
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
      lcd.setCursor(0, 0);
      lcd.print("Battery: ");
      lcd.print(UART.data.inpVoltage);
  
  }
  delay(50);
}
