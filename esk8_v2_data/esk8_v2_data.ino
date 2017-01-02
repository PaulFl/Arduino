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

struct bldcMeasure measuredValues;
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
  SERIALIO.begin(9600); //Vesc serial (Serial)
  lcd.begin(16, 2);
}

void loop() {
  delay(25);
  if (VescUartGetValue(measuredValues)) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Voltage: ");
    lcd.print(measuredValues.inpVoltage);
    lcd.setCursor(0, 1);
    lcd.print("Ah drawn: ");
    lcd.print(measuredValues.ampHours);
  }
}
