#include <buffer.h>
#include <crc.h>
#include <datatypes.h>
#include <local_datatypes.h>
#include <printf.h>
#include <VescUart.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal.h>

#define SERIALIO Serial


LiquidCrystal lcd(2, 4, 7, 8, 12, A0);
int page = 0;
int input;
int maxSpeed = 0;

struct bldcMeasure measuredValues;
SoftwareSerial sensitiveLink(A1, A2);
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

void setup() {
  sensitiveLink.begin(9600);
  SERIALIO.begin(9600); //Vesc serial (Serial)
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Voltage: ");
  lcd.setCursor(0, 1);
  lcd.print("Ah drawn: ");
}

void loop() {
  delay(5);
  if (VescUartGetValue(measuredValues)) {
    if (measuredValues.rpm * 60 * 15 * 3.1415926536 * 0.000083 / 36 / 7 > maxSpeed) {
      maxSpeed = measuredValues.rpm * 60 * 15 * 3.1415926536 * 0.000083 / 36 / 7;
    }
    switch (page) {
      case 0:
        lcd.setCursor(0, 0);
        lcd.print("Voltage: ");
        lcd.print(measuredValues.inpVoltage);
        lcd.print("V        ");
        lcd.setCursor(0, 1);
        lcd.print("Ah drawn: ");
        lcd.print(measuredValues.ampHours);
        lcd.print("Ah        ");
        break;
      case 2:
        lcd.setCursor(0, 0);
        lcd.print("Avg Batt: ");
        lcd.print(measuredValues.avgInputCurrent);
        lcd.print("A        ");
        lcd.setCursor(0, 1);
        lcd.print("Avg Motor: ");
        lcd.print(measuredValues.avgMotorCurrent);
        lcd.print("A        ");
        break;
      case 1:
        lcd.setCursor(0, 0);
        lcd.print("Max Speed: ");
        lcd.print(maxSpeed);
        lcd.print("kph        ");
        lcd.setCursor(0, 1);
        lcd.print("Dist: ");
        lcd.print((measuredValues.tachometer / 14) * 15 / 36 * 3.1415926536 * 0.000083);
        lcd.print("km        ");
        break;
      case 3:
        lcd.setCursor(0, 0);
        lcd.print("Ah drawn: ");
        lcd.print(measuredValues.ampHours);
        lcd.print("Ah        ");
        lcd.setCursor(0, 1);
        lcd.print("Ah regen: ");
        lcd.print(measuredValues.ampHoursCharged);
        lcd.print("Ah        ");
        break;

    }
  }
  input = 0;
  if (sensitiveLink.available()) {
    while (sensitiveLink.available()) {
      input = sensitiveLink.read();
    }
    if (input == 2) {
      page++;
      if (page == 4) {
        page = 0;
      }
    }
    if (input == 1) {
      page --;
      if (page == -1) {
        page = 3;
      }
    }
  }
}
