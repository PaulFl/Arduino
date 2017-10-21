#include <buffer.h>
#include <crc.h>
#include <datatypes.h>
#include <local_datatypes.h>
#include <printf.h>
#include <VescUart.h>

#include <SoftwareSerial.h>

 /* Values:
  *  avgMotorCurrent
  *  avgInputCurrent
  *  dutyCycleNow
  *  rpm
  *  inpVoltage
  *  ampHours
  *  ampHoursCharged
  *  tachometer
  *  tachometerAbs  
  */
#define SERIALIO vescSerial
struct bldcMeasure measuredValues;

SoftwareSerial vescSerial(7,8); //rx, tx

void setup() {
  Serial.begin(9600); 
  vescSerial.begin(9600);
  Serial.println("initialisation");
}

void loop() {
  if (VescUartGetValue(measuredValues)){
    //Serial.println(measuredValues.inpVoltage);
  }
}
