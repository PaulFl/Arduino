#include <buffer.h>
#include <crc.h>
#include <datatypes.h>
#include <local_datatypes.h>
#include <printf.h>
#include <VescUart.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>

#define SERIALIO Serial
#define SCHEMA 0x0111



LiquidCrystal lcd(2, 4, 7, 8, 12, A0);
int page = 0;
int input;
int maxSpeed = 0;
float totalKm = 0;
float totalChargeKm = 0;
float totalAh = 0;

unsigned long stoppedTime = 0;
unsigned long rollingTime = 0;
unsigned long lastStop;
unsigned long startTime;

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
template <class T> int EEPROM_writeAnything(int ee, const T& value)
{
  const byte* p = (const byte*)(const void*)&value;
  int i;
  for (i = 0; i < sizeof(value); i++)
    EEPROM.write(ee++, *p++);
  return i;
}


template <class T> int EEPROM_readAnything(int ee, T& value)
{
  byte* p = (byte*)(void*)&value;
  int i;
  for (i = 0; i < sizeof(value); i++)
    *p++ = EEPROM.read(ee++);
  return i;
}


void setup() {
  short schema;
  EEPROM_readAnything(sizeof(long) + sizeof(int) + 30, schema);
  if (schema != SCHEMA) {
    EEPROM_writeAnything( sizeof(long) + 1, (float(230)));
    schema = SCHEMA;
    EEPROM_writeAnything(sizeof(long) + sizeof(int) + 30, schema);
  }
  sensitiveLink.begin(9600);
  SERIALIO.begin(9600); //Vesc serial (Serial)
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
  lastStop = millis();
  startTime = millis();
  EEPROM_readAnything( sizeof(long) + 1, totalKm);
  EEPROM_readAnything( sizeof(long) + 10, totalAh);
  EEPROM_readAnything( sizeof(long) + 20, totalChargeKm);
  for (int i = 1; i<=4; i++){
    for(int j = 1; j<=5; j++){
      delay(3000/20);
      lcd.setCursor(11+i,1);
      lcd.write(byte(j));
    }
  }
}

void loop() {
  delay(50);
  EEPROM_writeAnything( sizeof(long) + 1, (totalKm + (float)((measuredValues.tachometer / 44) * 15 / 36 * 3.1415926536 * 0.000083) ));
  EEPROM_writeAnything( sizeof(long) + 10, (totalAh + (float)(measuredValues.ampHours)));
  EEPROM_writeAnything( sizeof(long) + 20, (totalChargeKm + (float)((measuredValues.tachometer / 44) * 15 / 36 * 3.1415926536 * 0.000083) ));
  if (VescUartGetValue(measuredValues)) {
    if (measuredValues.inpVoltage > 41) {
      totalAh = 0;
      totalChargeKm = 0;
    }

    if (measuredValues.rpm * 60 * 15 * 3.1415926536 * 0.000083 / 36 / 7 > maxSpeed) {
      maxSpeed = measuredValues.rpm * 60 * 15 * 3.1415926536 * 0.000083 / 36 / 7;
    }
    if (measuredValues.rpm * 60 * 15 * 3.1415926536 * 0.000083 / 36 / 7 <= 2) {
      stoppedTime += 5;
    }
    rollingTime = millis() - startTime;

    unsigned long timeElapsed = rollingTime / 1000 / 60;
    int rollTime = int(timeElapsed);

    switch (page) {
      case 0:
        lcd.setCursor(0, 0);
        lcd.print("Voltage: ");
        lcd.print(measuredValues.inpVoltage);
        lcd.print("V        ");
        lcd.setCursor(0, 1);
        lcd.print("Cycle: ");
        lcd.print(totalChargeKm + (float)((measuredValues.tachometer / 44) * 15 / 36 * 3.1415926536 * 0.000083));
        lcd.print("km        ");
        break;
      case 1:
        lcd.setCursor(0, 0);
        lcd.print("Total: ");
        lcd.print((totalKm + (float)((measuredValues.tachometer / 44) * 15 / 36 * 3.1415926536 * 0.000083) ));
        lcd.print("km        ");
        lcd.setCursor(0, 1);
        lcd.print("Dist: ");
        lcd.print((measuredValues.tachometer / 44) * 15 / 36 * 3.1415926536 * 0.000083);
        lcd.print("km        ");
        break;
      case 2:
        lcd.setCursor(0, 0);
        lcd.print("Ah drawn: ");
        lcd.print(measuredValues.ampHours);
        lcd.print("Ah        ");
        lcd.setCursor(0, 1);
        lcd.print("Ah regen: ");
        lcd.print(measuredValues.ampHoursCharged);
        lcd.print("Ah        ");
        break;
      case 3:
        lcd.setCursor(0, 0);
        lcd.print("Max Speed: ");
        lcd.print(maxSpeed);
        lcd.print("kph        ");
        lcd.setCursor(0, 1);
        lcd.print("Time: ");
        lcd.print(rollTime);
        lcd.print("min        ");
        break;
      case 4:
        lcd.setCursor(0, 0);
        lcd.print("Ride Wh: ");
        lcd.print((measuredValues.ampHours - measuredValues.ampHoursCharged) * measuredValues.inpVoltage);
        lcd.print("Wh          ");
        lcd.setCursor(0, 1);
        lcd.print("Avg: ");
        lcd.print(((measuredValues.ampHours - measuredValues.ampHoursCharged) * measuredValues.inpVoltage) / ((measuredValues.tachometer / 44) * 15 / 36 * 3.1415926536 * 0.000083));
        lcd.print("Wh/km       ");
        break;
      case 5:
        lcd.setCursor(0, 0);
        lcd.print("Cycle Wh: ");
        lcd.print((totalAh + measuredValues.ampHours) * 37);
        lcd.print("Wh          ");
        lcd.setCursor(0, 1);
        lcd.print("Avg: ");
        lcd.print((totalAh + measuredValues.ampHours) * 37 / (totalChargeKm + (float)((measuredValues.tachometer / 44) * 15 / 36 * 3.1415926536 * 0.000083) ));
        lcd.print("Wh/km          ");
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
      if (page == 6) {
        page = 0;
      }
    }
    if (input == 1) {
      page --;
      if (page == -1) {
        page = 5;
      }
    }
  }
}
