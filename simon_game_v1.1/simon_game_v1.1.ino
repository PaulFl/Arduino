// change this to any new value to re-init EEPROM state
#define SCHEMA 0x0010

#include <EEPROM.h>

#include "functions.h"
#include "pitches.h"

#define SIZE_MAX     99   //taille maximale de la séquence aléatoire
#define LIMITE_TEMPS 2500 //la limite de temps pour répondre
#define SPEED_MAX      200  //pause entre chaque note jouée
#define SPEED_MIN 500
#define ACCELERATION 30
#define REFRESHSPEED 3


#define ACHAR 10
#define BCHAR 23
#define CCHAR 11
#define DCHAR 22
#define ECHAR 12
#define FCHAR 13
#define GCHAR 24
#define HCHAR 14
#define ICHAR 15
#define JCHAR 16
#define KCHAR 25
#define LCHAR 17
#define MCHAR 26
#define NCHAR 18
#define OCHAR 0
#define PCHAR 19
#define QCHAR 27
#define RCHAR 28
#define SCHAR 5
#define TCHAR 29
#define UCHAR 20
#define VCHAR 30
#define WCHAR 31
#define XCHAR 32
#define YCHAR 33
#define ZCHAR 5


#define OFFSEG 21

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
const short boutons[] = {
  10, 12, A1, A3
};
const char buzzer  = 13;
const int frequences[] = {
  NOTE_E4, NOTE_CS4, NOTE_A4, NOTE_E3
};



boolean etats[] = {
  LOW, LOW, LOW, LOW
};
boolean mem[]  = {
  HIGH, HIGH, HIGH, HIGH
};

int sequence[SIZE_MAX];
int cpt = 0;
int score = 0;
int bestScore;
int bestScoreName_1;
int bestScoreName_2;
int pressed;

long debutNote = 0;
long resteTemps = 0;

boolean chars[34][7] = {{1, 1, 1, 1, 1, 1, 0}, // 0
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
  {0, 0, 1, 0, 1, 0, 1}, //n
  {1, 1, 0, 0, 1, 1, 1}, //P
  {0, 1, 1, 1, 1, 1, 0}, //U
  {0, 0, 0, 0, 0, 0, 0},//Off segment
  {0, 1, 1, 1, 1, 0, 1}, //d
  {0, 0, 1, 1, 1, 1, 1},//b
  {1, 0, 1, 1, 1, 1, 0},//G
  {1, 0, 1, 0, 1, 1, 1},//K
  {1, 1, 0, 1, 0, 1, 0},//M
  {1, 1, 1, 0, 0, 1, 1},//q
  {0, 0, 0, 0, 1, 0, 1},//r
  {0, 0, 0, 1, 1, 1, 1},//t
  {0, 1, 0, 1, 0, 1, 0},//V
  {0, 1, 1, 1, 1, 1, 1}, //W
  {1, 0, 0, 1, 0, 0, 1}, //X
  {0, 1, 1, 1, 0, 1, 1} //y
};

int alphabet[26] = {ACHAR, BCHAR, CCHAR, DCHAR, ECHAR, FCHAR, GCHAR, HCHAR, ICHAR, JCHAR, KCHAR, LCHAR, MCHAR, NCHAR, OCHAR, PCHAR, QCHAR, RCHAR, SCHAR, TCHAR, UCHAR, VCHAR, WCHAR, XCHAR, YCHAR, ZCHAR};

unsigned long flipmillis;

boolean segments[2][7] = {false};
byte onSegment = 0;


template <class T> int EEPROM_writeAnything(int ee, const T& value)
{
  const byte* p = (const byte*)(const void*)&value;
  int i;
  for (i = 0; i < sizeof(value); i++)
    EEPROM.write(ee++, *p++);
  return i;
}


template <class T> int EEPROM_readAnything(int ee, T& value)
{
  byte* p = (byte*)(void*)&value;
  int i;
  for (i = 0; i < sizeof(value); i++)
    *p++ = EEPROM.read(ee++);
  return i;
}


void BetterRandomSeed( void )
{
  unsigned long __seed;

  EEPROM_readAnything( 0, __seed );

  srandom( __seed );
}


long BetterRandom( void )
{
  long __seed;

  __seed = random();

  EEPROM_writeAnything( 0, __seed );

  return ( __seed );
}


long BetterRandom( long howbig )
{
  if ( howbig == 0 )
  {
    return 0;
  }
  return ( BetterRandom() % howbig );
}


long BetterRandom( long howsmall, long howbig) {
  if ( howsmall >= howbig )
  {
    return howsmall;
  }
  long diff = howbig - howsmall;

  return ( BetterRandom(diff) + howsmall );
}


void setup() {
  short schema;
  EEPROM_readAnything(sizeof(long) + sizeof(int) + 2, schema);
  if (schema != SCHEMA) {
    EEPROM_writeAnything(sizeof(long) + 1, int(0));
    schema = SCHEMA;
    EEPROM_writeAnything(sizeof(long) + sizeof(int) + 2, schema);
    EEPROM_writeAnything(sizeof(long) + 3 * (sizeof(int) + 1), PCHAR);
    EEPROM_writeAnything(sizeof(long) + 4 * (sizeof(int) + 1), FCHAR);
  }
  BetterRandomSeed();
  EEPROM_readAnything( sizeof(long) + 1, bestScore);
  EEPROM_readAnything(sizeof(long) + 3 * (sizeof(int) + 1), bestScoreName_1);
  EEPROM_readAnything( sizeof(long) + 4 * (sizeof(int) + 1), bestScoreName_2);
  for (short i = 0; i < 4; i++) {
    pinMode(leds[i],    OUTPUT);
    pinMode(boutons[i], INPUT);
    digitalWrite(leds[i],    LOW);
    digitalWrite(boutons[i], HIGH);
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
  boolean perdu = false;
  char nbEssai = 0;
  char mancheOK = false;

  while (!perdu && ajouterEtape()) {
    nbEssai = 0;
    mancheOK = false;
    while (nbEssai < 1 && !mancheOK) {
      delayFlip(1000);
      playSequence(false);

      mancheOK = true;
      for (short i = 0; i < cpt; i++) {
        pressed = checkEtape(i);
        if (pressed != -1) {
          mancheOK = false;
          break;
        }
      }
      if (!mancheOK) {
        nbEssai++;
        animationReponseFausse(pressed);
      }
      else {
        delayFlip(175);
      }
      delayFlip(500);
    }

    if (nbEssai == 1) {
      perdu = true;
    }
  }

  ecranFin(perdu);
}

void flip() {
  if (millis() - flipmillis > REFRESHSPEED) {
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

boolean ajouterEtape() {
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

void playSequence(boolean finished) {
  short lenght;
  if (finished) lenght = cpt - 1;
  else lenght = cpt;
  for (short i = 0; i < lenght; i++) {
    digitalWrite(leds[sequence[i]], HIGH);
    tone(buzzer, frequences[sequence[i]]);
    delayFlip(SPEED_MAX);
    if (score < ((SPEED_MIN - SPEED_MAX) / ACCELERATION) && !finished) {
      delayFlip(SPEED_MIN - SPEED_MAX - (ACCELERATION * score));
    }
    else if (finished) {
      delayFlip(400 - SPEED_MAX);
    }
    digitalWrite(leds[sequence[i]], LOW);
    noTone(buzzer);
    delayFlip(SPEED_MAX / 3);
    if (score < ((SPEED_MIN - SPEED_MAX) / ACCELERATION) && !finished) {
      delayFlip((SPEED_MIN - SPEED_MAX - (ACCELERATION * score)) / 3);
    }
    else if (finished) {
      delayFlip((400 - SPEED_MAX) / 2);
    }
  }
}

int checkEtape(char etape) {
  long temps = millis();
  char objectif = sequence[etape];

  while ((millis() - temps) < (LIMITE_TEMPS + resteTemps)) {
    flip();
    lectureBoutons();
    if (millis() - debutNote > 500) {
      noTone(buzzer);
      for (short i = 0; i < 4; i++) {
        digitalWrite(leds[i],    LOW);
      }
    }
    for (int i = 0; i < 4; i++) {
      if (etats[i]) {
        if (i != objectif) {
          noTone(buzzer);
          for (short i = 0; i < 4; i++) {
            digitalWrite(leds[i],    LOW);
          }
          return i;
        }
        else if (i == objectif) {
          noTone(buzzer);
          for (short i = 0; i < 4; i++) {
            digitalWrite(leds[i],    LOW);
          }
          delayFlip(10);
          //resteTemps = LIMITE_TEMPS - millis() + temps;
          digitalWrite(leds[i], HIGH);
          tone(buzzer, frequences[i]);
          debutNote = millis();
          delayFlip(200);
          if (etape == cpt - 1) {
            delayFlip(500);
            digitalWrite(leds[i], LOW);
            noTone(buzzer);
            resteTemps = 0;
          }
          return -1;
        }
      }
    }
  }
  return objectif;
}

void lectureBoutons() {
  for (short i = 0; i < 4; i++) {
    char etat = digitalRead(boutons[i]);
    if (etat != mem[i] ) {
      if (etat == LOW) {
        etats[i] = true;
        mem[i] = LOW;
      } else {
        mem[i] = HIGH;
        etats[i] = false;
      }
    } else {
      etats[i] = false;
    }
  }
}

void animationReponseFausse(int lastPressed) {
  digitalWrite(leds[lastPressed], HIGH);
  tone(buzzer, NOTE_A1);
  delayFlip(750);
  digitalWrite(leds[lastPressed], LOW);
  noTone(buzzer);
  delayFlip(500);
}

void ecranFin(boolean perdu) {
  if (perdu) {
    if (score > bestScore) {
      EEPROM_writeAnything( sizeof(long) + 1, score);
      bestScore = score;
      byte array[2];
      array[0] = ACHAR;
      array[1] = OFFSEG;
      display(array);
      bool nameValid = false;
      bool currentSeg = 0;
      int nameCharIndex[2] = {0,0};
      while (!nameValid) {
        flip();
        lectureBoutons();
        if (etats[0]) {
          nameCharIndex[currentSeg]--;
          if (nameCharIndex[currentSeg] < 0) {
            nameCharIndex[currentSeg] = 25;
          }
          array[currentSeg] = alphabet[nameCharIndex[currentSeg]];
          display(array);
        } else if (etats[1]) {
          nameCharIndex[currentSeg]++;
          if (nameCharIndex[currentSeg] > 25) {
            nameCharIndex[currentSeg] = 0;
          }
          array[currentSeg] = alphabet[nameCharIndex[currentSeg]];
          display(array);
        } else if (etats[2]) {
          array[currentSeg] = OFFSEG;
          currentSeg = (currentSeg == 0) ? 1 : 0;
          array[currentSeg] = alphabet[nameCharIndex[currentSeg]];
          display(array);
        } else if (etats[3]) {
          nameValid = true;
        }
        delayFlip(10);
      }
      bestScoreName_1 = alphabet[nameCharIndex[0]];
      bestScoreName_2 = alphabet[nameCharIndex[1]];
      EEPROM_writeAnything(sizeof(long) + 3 * (sizeof(int) + 1), bestScoreName_1);
      EEPROM_writeAnything(sizeof(long) + 4 * (sizeof(int) + 1), bestScoreName_2);
      
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
      //joue la séquence si on lit un état bas
      if (!digitalRead(boutons[0]) || !digitalRead(boutons[1]) || !digitalRead(boutons[2]) || !digitalRead(boutons[3])) {
        playSequence(true);
      }
    }
  }
}

void resetScore() {
  if (!digitalRead(boutons[0]) && !digitalRead(boutons[3])) {
    bestScore = 0;
    EEPROM_writeAnything( sizeof(long) + 1, bestScore);
    EEPROM_writeAnything(sizeof(long) + 3 * (sizeof(int) + 1), PCHAR);
    EEPROM_writeAnything(sizeof(long) + 4 * (sizeof(int) + 1), FCHAR);
  }
}
