// change this to any new value to re-init EEPROM state
#define SCHEMA 0x0010

#define SOFT_VERSION "2.5"

/*
 EEPROM map: (adress: byte) (1 byte = 8 bits)
 0: software main version (int) (2 bytes)
 2: software sub version (int) (2 bytes)
 4: schema (short) (2 bytes)
 6: seed for random (long) (4 bytes)
 10: best score (int) (2 bytes)
 12: best score name 1 (int) (2 bytes)
 14: best score name 2 (int) (2 bytes)
 */

#define EEPROM_ADDR_SW_1 0
#define EEPROM_ADDR_SW_2 2
#define EEPROM_ADDR_SCHEMA 4
#define EEPROM_ADDR_SEED 6
#define EEPROM_ADDR_SCORE 10
#define EEPROM_ADDR_SCORE_NAME_1 12
#define EEPROM_ADDR_SCORE_NAME_2 14

#include "EEPROM_rw.h"
#include "segments_display.h"
#include "functions.h"
#include "pitches.h"
#include "random_eeprom.h"



#define SIZE_MAX     99   //max game length
#define MAX_RESPONSE_DELAY 2500
#define MIN_SPEED_DELAY 500
#define MAX_SPEED_DELAY      200
#define DELAY_BEETWEEN_PRESSES 200
#define DELAY_DEBOUNCE 20
#define ACCELERATION_RATE 30
#define SEGMENTS_REFRESH_DELAY 3


//Pins
short cathodeSegments[2] = {0, //SEG 1
    1
}; //SEG 2

short anodeSegments[7] = {5, //SEG A
    2, //SEG B
    6, //SEG C
    3, //SEG D
    7, //SEG E
    4, //SEG F
    8
}; //SEG G

const short leds[]    = {
    9, 11, A0, A2
};
const short buttons[] = {
    10, 12, A1, A3
};
const short buzzer  = 13;


const int frequencies[] = {
    NOTE_E4, NOTE_CS4, NOTE_A4, NOTE_E3
    //NOTE_E3, NOTE_CS4, NOTE_E4, NOTE_A4  //In order
};

bool states[] = {
    LOW, LOW, LOW, LOW
};

bool mem[]  = {
    HIGH, HIGH, HIGH, HIGH
};

int sequence[SIZE_MAX];
int cpt = 0;
int score = 0;
int bestScore;
int bestScoreName_1;
int bestScoreName_2;
int pressed;

long noteStart = 0;
long timeRemaining = 0;

unsigned long flipmillis;
unsigned long buttonMillis[4] = {0, 0, 0, 0};

bool segments[2][7] = {false};
byte onSegment = 0;


void setup() {
    EEPROM_writeAnything(EEPROM_ADDR_SW_1, SOFT_VERSION[0] - '0');
    EEPROM_writeAnything(EEPROM_ADDR_SW_2, SOFT_VERSION[2] - '0');
    short schema;
    EEPROM_readAnything(EEPROM_ADDR_SCHEMA, schema);
    if (schema != SCHEMA) {
        EEPROM_writeAnything(EEPROM_ADDR_SCORE, int(0)); //Best score
        schema = SCHEMA;
        EEPROM_writeAnything(EEPROM_ADDR_SCHEMA, schema);
        EEPROM_writeAnything(EEPROM_ADDR_SCORE_NAME_1, int(PCHAR));
        EEPROM_writeAnything(EEPROM_ADDR_SCORE_NAME_2, int(FCHAR));
    }
    BetterRandomSeed();
    EEPROM_readAnything(EEPROM_ADDR_SCORE, bestScore);
    EEPROM_readAnything(EEPROM_ADDR_SCORE_NAME_1, bestScoreName_1);
    EEPROM_readAnything(EEPROM_ADDR_SCORE_NAME_2, bestScoreName_2);
    for (short i = 0; i < 4; i++) {
        pinMode(leds[i],    OUTPUT);
        pinMode(buttons[i], INPUT);
        digitalWrite(leds[i],    LOW);
        digitalWrite(buttons[i], HIGH);
        buttonMillis[i] = millis();
    }
    for (short i = 0; i < 2; i++) {
        pinMode(cathodeSegments[i], OUTPUT);
    }
    for (short i = 0; i < 7; i++) {
        pinMode(anodeSegments[i], OUTPUT);
    }
    pinMode(1, OUTPUT);
    digitalWrite(1, HIGH);
    pinMode(buzzer, OUTPUT);
    
    resetScore();
    displaySoftVersion();
    
    byte array[2];
    array[0] = bestScoreName_1;
    array[1] = bestScoreName_2;
    display(array);
    delayFlip(1000);
    
    if (bestScore < 10) {
        array[0] = OFFSEG;
    }
    else {
        array[0] = bestScore / 10;
    }
    array[1] = bestScore % 10;
    display(array);
    delayFlip(2000);
}

void loop() {
    bool lost = false;
    char tryNumber = 0;
    char roundOk = false;
    
    while (!lost && addStep()) {
        tryNumber = 0;
        roundOk = false;
        while (tryNumber < 1 && !roundOk) {
            delayFlip(1000);
            playSequence(false);
            
            roundOk = true;
            for (short i = 0; i < cpt; i++) {
                pressed = checkEtape(i);
                if (pressed != -1) {
                    roundOk = false;
                    break;
                }
            }
            if (!roundOk) {
                tryNumber++;
                mistakeAnimation(pressed);
            }
            else {
                delayFlip(175);
            }
            delayFlip(500);
        }
        
        if (tryNumber == 1) {
            lost = true;
        }
    }
    
    ecranFin(lost);
}

void flip() {
    if (millis() - flipmillis > SEGMENTS_REFRESH_DELAY) {
        digitalWrite(cathodeSegments[onSegment], LOW);
        onSegment++;
        if (onSegment == 2) {
            onSegment = 0;
        }
        for (int i = 0; i < 7; i++) {
            digitalWrite(anodeSegments[i], segments[onSegment][i]);
        }
        digitalWrite(cathodeSegments[onSegment], HIGH);
        flipmillis = millis();
    }
}

void delayFlip(unsigned int delay) {
    unsigned long pastTime = millis();
    while ((millis() - pastTime) < delay) {
        flip();
    }
}

void display(byte text[2]) {
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 7; j++) {
            segments[i][j] = chars[text[i]][j];
        }
    }
}

bool addStep() {
    sequence[cpt] = BetterRandom(4);
    cpt++;
    score = (int)cpt - 1;
    byte array[2];
    if (score < 10) {
        array[0] = OFFSEG;
    }
    else {
        array[0] = score / 10;
    }
    array[1] = score % 10;
    display(array);
    if (cpt == SIZE_MAX)
        return false;
    else
        return true;
}

void playSequence(bool finished) {
    short length;
    if (finished) length = cpt - 1;
    else length = cpt;
    for (short i = 0; i < length; i++) {
        digitalWrite(leds[sequence[i]], HIGH);
        tone(buzzer, frequencies[sequence[i]]);
        delayFlip(MAX_SPEED_DELAY);
        if (score < ((MIN_SPEED_DELAY - MAX_SPEED_DELAY) / ACCELERATION_RATE) && !finished) {
            delayFlip(MIN_SPEED_DELAY - MAX_SPEED_DELAY - (ACCELERATION_RATE * score));
        }
        else if (finished) {
            delayFlip(400 - MAX_SPEED_DELAY);
        }
        digitalWrite(leds[sequence[i]], LOW);
        noTone(buzzer);
        delayFlip(MAX_SPEED_DELAY / 3);
        if (score < ((MIN_SPEED_DELAY - MAX_SPEED_DELAY) / ACCELERATION_RATE) && !finished) {
            delayFlip((MIN_SPEED_DELAY - MAX_SPEED_DELAY - (ACCELERATION_RATE * score)) / 3);
        }
        else if (finished) {
            delayFlip((400 - MAX_SPEED_DELAY) / 2);
        }
    }
}

int checkEtape(char etape) {
    long temps = millis();
    char goal = sequence[etape];
    
    while ((millis() - temps) < (MAX_RESPONSE_DELAY + timeRemaining)) {
        flip();
        readButtons();
        if (millis() - noteStart > 500) {
            noTone(buzzer);
            for (short i = 0; i < 4; i++) {
                digitalWrite(leds[i],    LOW);
            }
        }
        for (int i = 0; i < 4; i++) {
            if (states[i]) {
                if (i != goal) {
                    noTone(buzzer);
                    for (short i = 0; i < 4; i++) {
                        digitalWrite(leds[i],    LOW);
                    }
                    return -2;
                }
                else if (i == goal) {
                    noTone(buzzer);
                    for (short i = 0; i < 4; i++) {
                        digitalWrite(leds[i],    LOW);
                    }
                    delayFlip(10);
                    digitalWrite(leds[i], HIGH);
                    tone(buzzer, frequencies[i]);
                    noteStart = millis();
                    delayFlip(DELAY_BEETWEEN_PRESSES);
                    if (etape == cpt - 1) {
                        delayFlip(500);
                        digitalWrite(leds[i], LOW);
                        noTone(buzzer);
                        timeRemaining = 0;
                    }
                    return -1;
                }
            }
        }
    }
    return goal;
}

void readButtons() {
    for (short i = 0; i < 4; i++) {
        char etat = digitalRead(buttons[i]);
        if (millis() - buttonMillis[i] > DELAY_DEBOUNCE) {
            buttonMillis[i] = millis();
            if (etat != mem[i]) {
                if (etat == LOW) {
                    states[i] = true;
                    mem[i] = LOW;
                } else {
                    mem[i] = HIGH;
                    states[i] = false;
                }
            } else {
                states[i] = false;
            }
        }
    }
}

void mistakeAnimation(int lastPressed) {
    if (lastPressed != -2){
        digitalWrite(leds[lastPressed], HIGH);
        tone(buzzer, NOTE_A1);
        delayFlip(500);
    }
    for (short i = 0; i < 4; i++) {
        digitalWrite(leds[i],    HIGH);
    }
    tone(buzzer, NOTE_A1);
    
    if (lastPressed != -2){
        delayFlip(250);
        noTone(buzzer);
    } else {
        delayFlip(750);
        noTone(buzzer);
    }
    for (short i = 0; i < 4; i++) {
        digitalWrite(leds[i],    LOW);
    }
    delayFlip(500);
}

void ecranFin(bool lost) {
    if (lost) {
        if (score > bestScore) {
            EEPROM_writeAnything(EEPROM_ADDR_SCORE, score);
            bestScore = score;
            byte array[2];
            array[0] = ACHAR;
            array[1] = OFFSEG;
            display(array);
            bool nameValid = false;
            bool canValidate = false;
            bool currentSeg = 0;
            int nameCharIndex[2] = {0, 0};
            while (!nameValid) {
                flip();
                readButtons();
                if (states[0]) {
                    nameCharIndex[currentSeg]--;
                    if (nameCharIndex[currentSeg] < 0) {
                        nameCharIndex[currentSeg] = 25;
                    }
                    array[currentSeg] = alphabet[nameCharIndex[currentSeg]];
                    display(array);
                } else if (states[1]) {
                    nameCharIndex[currentSeg]++;
                    if (nameCharIndex[currentSeg] > 25) {
                        nameCharIndex[currentSeg] = 0;
                    }
                    array[currentSeg] = alphabet[nameCharIndex[currentSeg]];
                    display(array);
                } else if (states[2]) {
                    canValidate = true;
                    array[currentSeg] = OFFSEG;
                    currentSeg = (currentSeg == 0) ? 1 : 0;
                    array[currentSeg] = alphabet[nameCharIndex[currentSeg]];
                    display(array);
                } else if (states[3] && canValidate) {
                    nameValid = true;
                }
                delayFlip(30);
            }
            bestScoreName_1 = alphabet[nameCharIndex[0]];
            bestScoreName_2 = alphabet[nameCharIndex[1]];
            EEPROM_writeAnything(EEPROM_ADDR_SCORE_NAME_1, bestScoreName_1);
            EEPROM_writeAnything(EEPROM_ADDR_SCORE_NAME_2, bestScoreName_2);
            
            if (bestScore < 10) {
                array[0] = OFFSEG;
            }
            else {
                array[0] = bestScore / 10;
            }
            array[1] = bestScore % 10;
            display(array);
        }
        while (1) {
            flip();
            if (!digitalRead(buttons[0]) || !digitalRead(buttons[1]) || !digitalRead(buttons[2]) || !digitalRead(buttons[3])) { //replay sequence if any button pressed
                playSequence(true);
            }
        }
    }
}

void resetScore() {
    if (!digitalRead(buttons[0]) && !digitalRead(buttons[3])) {
        bestScore = 0;
        EEPROM_writeAnything(EEPROM_ADDR_SCORE, bestScore);
        EEPROM_writeAnything(EEPROM_ADDR_SCORE_NAME_1, int(PCHAR));
        EEPROM_writeAnything(EEPROM_ADDR_SCORE_NAME_2, int(FCHAR));
    }
}

void displaySoftVersion() {
    if (!digitalRead(buttons[1])) {
        short software_version[2] = {0,0};
        software_version[0] = SOFT_VERSION[0] - '0';
        software_version[1] = SOFT_VERSION[2] - '0';
        
        byte array[2];
        
        array[0] = SCHAR;
        array[1] = OCHAR;
        display(array);
        delayFlip(500);
        
        array[0] = FCHAR;
        array[1] = TCHAR;
        display(array);
        delayFlip(500);
        
        array[0] = software_version[0];
        array[1] = software_version[1];
        display(array);
        delayFlip(1500);
    }
}
