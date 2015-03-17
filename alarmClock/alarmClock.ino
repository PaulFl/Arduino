#include "segments.h"
#include "pins.h"

#define DEBUG 0
#define REFRESHSPEED 2



boolean chars[20][7] = {{1, 1, 1, 1, 1, 1, 0}, // 0
    {0, 1, 1, 0, 0, 0, 0}, //1
    {1, 1, 0, 1, 1, 0, 1}, //2
    {1, 1, 1, 1, 0, 0, 1}, //3
    {0, 1, 1, 0, 0, 1, 1}, //4
    {1, 0, 1, 1, 0, 1, 1}, //5
    {1, 0, 1, 1, 1, 1, 1}, //6
    {1, 1, 1, 0, 0, 0, 0}, //7
    {1, 1, 1, 1, 1, 1, 1}, //8
    {1, 1, 1, 1, 0, 1, 1}, //9
    {1, 1, 1, 0, 1, 1, 1}, //A
    {1, 0, 0, 1, 1, 1, 0}, //C
    {1, 0, 0, 1, 1, 1, 1}, //E
    {1, 0, 0, 0, 1, 1, 1}, //F
    {0, 1, 1, 0, 1, 1, 0}, //H
    {0, 0, 0, 0, 1, 1, 0}, //I
    {0, 1, 1, 1, 1, 0, 0}, //J
    {0, 0, 0, 1, 1, 1, 0}, //L
    {1, 1, 0, 0, 1, 1, 1}, //P
    {0, 1, 1, 1, 1, 1, 0}}; //U

unsigned long flipmillis;

boolean segments[6][8];
short onSegment = 0;

void setup() {
        Serial.begin(9600); //Init Serial
    //Init segments pins
    for (int i = 0; i < 6; i++) {
        pinMode(cathodeSegments[i], OUTPUT);
        digitalWrite(cathodeSegments[i], HIGH);
    }
    for (int i = 0; i < 8; i++) {
        pinMode(anodeSegments[i], OUTPUT);
        digitalWrite(anodeSegments[i], LOW);
    }
    
    //Play init sound
    
}

void loop() {
    
}

void display(short text[6]) {
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 7; j++) {
            segments[i][j] = chars[text[i]][j];
        }
    }
    
}

void setSeparators(boolean separators[6]) {
    for (int i = 0; i < 6; i++) {
        segments[i][7] = separators[i];
    }
}

void flip() {
    if (flipmillis - millis() > REFRESHSPEED) {
        digitalWrite(onSegment, HIGH);
        onSegment++;
        if (onSegment == 6) {
            onSegment = 0;
        }
        digitalWrite(onSegment, LOW);
        for (int i = 0; i < 8; i++) {
            digitalWrite(anodeSegments[i], segments[onSegment][i]);
        }
        flipmillis = millis();
    }
}
