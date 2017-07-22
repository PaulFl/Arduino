#include <buffer.h>
#include <crc.h>
#include <datatypes.h>
#include <local_datatypes.h>
#include <printf.h>
#include <VescUart.h>
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

struct bldcMeasure measuredValues;

void setup() {
  Serial.begin(9600); //Vesc serial
}

void loop() {
  if (VescUartGetValue(measuredValues)){
    
  }
}
