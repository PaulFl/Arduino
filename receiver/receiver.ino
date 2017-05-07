#include <nRF24L01.h>
#include <printf.h>
#include <RF24.h>
#include <RF24_config.h>
#include <Servo.h>


int msg[4];
RF24 radio(7, 8);
const uint64_t pipe = 0xE8E8F0F1E0LL;
short motorPin = 9;


Servo motor;
long lastRead;

void setup() {
  motor.attach(motorPin);
  motor.write(20);
  radio.begin();
  radio.openReadingPipe(1, pipe);
  radio.startListening();

  lastRead = millis();
  
}

void loop() {
  if (millis() - lastRead > 500) { // Let go if signal timeout
    motor.write(20);
  }
  if (radio.available()){
    while (radio.available()) radio.read(msg, 7);
    motor.write(map(msg[0], 0, 180, 25, 170));
    lastRead = millis();
  }
}

