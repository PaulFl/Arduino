#include <nRF24L01.h>
#include <printf.h>
#include <RF24.h>
#include <RF24_config.h>

int msg[4];
RF24 radio(7, 8);
const uint64_t pipe = 0xE8E8F0F0E1LL;


void setup(void) {
  Serial.begin(9600);
  radio.begin();
  radio.openReadingPipe(1, pipe);
  radio.startListening();
}

void loop(void) {
  if (radio.available()) {
    while (radio.available()) {
      radio.read(msg, 8);
    }
    Serial.print(msg[0]);
    Serial.print("\t");
    Serial.print(msg[1]);
    Serial.print("\t");
    Serial.print(msg[2]);
    Serial.print("\t");
    Serial.println(msg[3]);
  } 
}
