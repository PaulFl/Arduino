#include <nRF24L01.h>
#include <printf.h>
#include <RF24.h>
#include <RF24_config.h>

#define VERTICALMIN 0
#define VERTICALMAX 180
#define HORIZONTALMIN 0
#define HORIZONTALMAX 100

#define DELAY 20


#define VERTICAL 0
#define HORIZONTAL 1
#define CBUTTON 2
#define ZBUTTON 3

short remotePin = 9;


short RF24cePin = 7;
short RF24csnPin = 8;

int msg[4];
RF24 radio(RF24cePin, RF24csnPin);
const uint64_t pipe = 0xE8E8F0F0E1LL; //Paul
//const uint64_t pipe = 0xE8E8F0F1E0LL;//Aymeric

void setup() {
  radio.begin();
  radio.openWritingPipe(pipe);

  pinMode(remotePin, INPUT);
  
}

void loop() {
  delay(DELAY);
  
  int lecture = pulseIn(remotePin, HIGH);
  if (lecture >= 1000){
  msg[VERTICAL] = map(lecture, 1000, 2000, 0, 180);
  } else {
    msg[VERTICAL] = 90;
  }
  msg[HORIZONTAL] = 50;
  
  msg[CBUTTON] = 1;
  msg[ZBUTTON] = 1;
  
  radio.write(msg,8);
}
