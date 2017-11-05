#include <nRF24L01.h>
#include <printf.h>
#include <RF24.h>
#include <RF24_config.h>

#define MSGLENGTH 4 //nunchuck 4, ballance robot 5

int msg[MSGLENGTH];
RF24 radio(7, 8);
//const uint64_t pipe = 0xE8E8F0F1E0LL;// RED remote
const uint64_t pipe = 0xE8E8F0F0E1LL;//White remote, ballance robot


void setup(void) {
  Serial.begin(9600);
  radio.begin();
  radio.openReadingPipe(1, pipe);
  radio.startListening();
}

void loop(void) {
  if (radio.available()) {
    while (radio.available()) {
      radio.read(msg, 2 * MSGLENGTH);
    }
    for (int i = 0; i < MSGLENGTH; i++) {
      Serial.print(msg[i]);
      Serial.print("\t");
    }
    Serial.println(" ");
  }
}
