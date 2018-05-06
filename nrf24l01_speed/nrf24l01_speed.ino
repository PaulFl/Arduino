#include <nRF24L01.h>
#include <printf.h>
#include <RF24.h>
#include <RF24_config.h>

#define MSGLENGTH 4

int msg[MSGLENGTH];

RF24 radio(7,8);

const uint64_t pipe = 0xE8E8F0F0E1LL;

long rcvTime;

void setup(){
  Serial.begin(9600);
  radio.begin();
  radio.openReadingPipe(1, pipe);
  radio.startListening();
  rcvTime = millis();
}

void loop(){
  if (radio.available()) {
    while (radio.available()) {
      radio.read(msg, 2*MSGLENGTH);
    }
    long currentTime = millis();
    Serial.print(currentTime - rcvTime);
    Serial.print("\t");
    for (int i = 0; i < MSGLENGTH; i++) {
      Serial.print(msg[i]);
      Serial.print("\t");
    }
    Serial.println();
    rcvTime = currentTime;
  }
}

