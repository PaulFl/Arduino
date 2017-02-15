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

unsigned long stoppedTime = 0;
unsigned long rollingTime = 0;
unsigned long lastStop;
unsigned long startTime;

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
  lastStop = millis();
  startTime = millis();
}

void loop() {
  delay(5);
  if (VescUartGetValue(measuredValues)) {

    if (measuredValues.rpm * 60 * 15 * 3.1415926536 * 0.000083 / 36 / 7 > maxSpeed) {
      maxSpeed = measuredValues.rpm * 60 * 15 * 3.1415926536 * 0.000083 / 36 / 7;
    }
    if (measuredValues.rpm * 60 * 15 * 3.1415926536 * 0.000083 / 36 / 7 <= 2) {
      stoppedTime += 5;
    }
    rollingTime = millis() - startTime - stoppedTime;

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
        lcd.print((measuredValues.tachometer / 44) * 15 / 36 * 3.1415926536 * 0.000083);
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
      case 4:
        lcd.setCursor(0, 0);
        lcd.print("Time: ");
        lcd.print(rollingTime/1000/60);
        lcd.print("min        ");
        lcd.setCursor(0, 1);
        lcd.print("Avg Speed: ");
        lcd.print((measuredValues.tachometer / 44) * 15 / 36 * 3.1415926536 * 0.000083 / ((float)rollingTime / 1000.0 / 60.0 / 60.0));
        lcd.print("        ");
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
      if (page == 5) {
        page = 0;
      }
    }
    if (input == 1) {
      page --;
      if (page == -1) {
        page = 4;
      }
    }
  }
}
