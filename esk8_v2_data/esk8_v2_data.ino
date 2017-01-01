#include <SoftwareSerial.h>
#include <LiquidCrystal.h>

LiquidCrystal lcd(2,4,7,8,12,A0);

void setup() {
  lcd.begin(16,2);
  lcd.print("test");
}

void loop() {
  // put your main code here, to run repeatedly:

}
