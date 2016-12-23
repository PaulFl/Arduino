#include <nRF24L01.h>
#include <printf.h>
#include <RF24.h>
#include <RF24_config.h>
#include <Servo.h>


int msg[1];
RF24 radio(7, 8);
const uint64_t pipe = 0xE8E8F0F0E1LL;

Servo motor;
long lastRead;


void setup(void) {
  Serial.begin(9600);
  radio.begin();
  radio.openReadingPipe(1, pipe);
  radio.startListening();
  motor.attach(9);
  motor.write(15);
  lastRead = millis();
}

void loop(void) {
  if (millis() - lastRead > 400 && motor.read()>15) {
    motor.write(motor.read() - 3);
  }
  if (radio.available()) {
    while (radio.available()) radio.read(msg, 2);
    if (msg[0] <= 180 && msg[0] >= 20) {
      motor.write(map(msg[0], 20, 180, 15, 180));
                  lastRead = millis();
    } else if (motor.read() > 15){
      motor.write(motor.read() - 3);
    }

  }

}
