#include <EEPROM.h>

#include "functions.h"

#define SIZE_MAX     99   //taille maximale de la séquence aléatoire
#define LIMITE_TEMPS 2500 //la limite de temps pour répondre
#define SPEED_MAX      250  //pause entre chaque note jouée
#define SPEED_MIN 500
#define ACCELERATION 10

#include "pitches.h"

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

	    return( __seed );
}


long BetterRandom( long howbig )
{
	 if ( howbig == 0 ) 
		  {
			     return 0;
			      }
	  return( BetterRandom() % howbig );
}


long BetterRandom( long howsmall, long howbig) {
	 if ( howsmall >= howbig ) 
		  {
			     return howsmall;
			      }
	  long diff = howbig - howsmall;
	   
	   return( BetterRandom(diff) + howsmall );
}

const short leds[]    = {
  9, 12, A2, A5};
const short boutons[] = {
  10, 11, A3, A4};
const char buzzer  = 13;
const int frequences[] = {
  NOTE_E4, NOTE_CS4, NOTE_A4, NOTE_E3};


 
boolean etats[] = {
  LOW,LOW,LOW, LOW};
boolean mem[]  = {
  HIGH,HIGH,HIGH, HIGH};
 
int sequence[SIZE_MAX];
int cpt = 0;
int score = 0;
int pressed;
 
 
void setup() {
    BetterRandomSeed();
    for(short i=0; i<4; i++) {
    pinMode(leds[i],    OUTPUT);
    pinMode(boutons[i], INPUT);
    digitalWrite(leds[i],    LOW);
    digitalWrite(boutons[i], HIGH);
  }
  pinMode(buzzer, OUTPUT);
  
  //7 segments display---

  //---------------------
 
 
  Serial.begin(14400);
  Serial.println("Initialisation...");
  delay(50);
}
 
void loop() {
  boolean perdu = false;
  char nbEssai = 0;
  char mancheOK = false;
 
  while(!perdu && ajouterEtape()) {
    nbEssai = 0;
    mancheOK = false;
    while(nbEssai < 1 && !mancheOK) {
      delay(1000);
      playSequence(false);    
      
      mancheOK = true;
      for(short i=0; i<cpt; i++) {
        pressed = checkEtape(i);
        if(pressed != -1) {
          mancheOK = false;
          break;
        }
      }
      if(!mancheOK) {
        nbEssai++;
        animationReponseFausse(pressed);
      } 
      else {
        delay(175); 
      }
      delay(500);
    }
    
    if(nbEssai == 1) {
      perdu = true;
    }
  }
 
  ecranFin(perdu);
}


boolean ajouterEtape() {
  sequence[cpt] = BetterRandom(4);
  cpt++;
  score = (int)cpt - 1;
  Serial.print(score);
  if(cpt == SIZE_MAX)
    return false;
  else
    return true;
}
 
void playSequence(boolean finished) {
  short lenght;
  if(finished) lenght = cpt-1;
  else lenght = cpt;
  for(short i=0; i<lenght; i++) {
    digitalWrite(leds[sequence[i]], HIGH);
    tone(buzzer, frequences[sequence[i]]);
    delay(SPEED_MAX);
    if(score < ((SPEED_MIN-SPEED_MAX)/ACCELERATION) && !finished) {
      delay(SPEED_MIN-SPEED_MAX - (ACCELERATION*score));
    }
    else if (finished) {
      delay(400 - SPEED_MAX);
    }
    digitalWrite(leds[sequence[i]], LOW); 
    noTone(buzzer);
    delay(SPEED_MAX);
    if(score < ((SPEED_MIN-SPEED_MAX)/ACCELERATION) && !finished) {
      delay(SPEED_MIN-SPEED_MAX - (ACCELERATION*score));
    }
    else if (finished) {
      delay(400 - SPEED_MAX);
    }
  }
}
 
int checkEtape(char etape) {
  long temps = millis();
  char objectif = sequence[etape];
 
  while((millis() - temps) < LIMITE_TEMPS) {
    lectureBoutons();
    for(int i=0; i<4; i++) {
      if(etats[i]) {
	if(i != objectif) {
		return i;
	}
	else if(i == objectif) {
        digitalWrite(leds[i], HIGH);
        tone(buzzer, frequences[i]);
        delay(400);
        digitalWrite(leds[i], LOW);
        noTone(buzzer);
	return -1;
	}
	} 
        }
  }
  return objectif;
 }
 
void lectureBoutons() {
  for(short i=0; i<4; i++) {
    char etat = digitalRead(boutons[i]);
    if(etat != mem[i] ) {
      if(etat == LOW) {
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
   digitalWrite(leds[sequence[lastPressed]], HIGH); 
    tone(buzzer, NOTE_A2);
    delay(750);
    digitalWrite(leds[sequence[lastPressed]], LOW);
    noTone(buzzer);
    delay(500);
}
 
void ecranFin(boolean perdu) {
  if(perdu) {
    while(1) {
      //joue la séquence si on lit un état bas
      if(!digitalRead(boutons[0]) || !digitalRead(boutons[1]) || !digitalRead(boutons[2]) || !digitalRead(boutons[3])) {
        playSequence(true); 
      }
    }
  }
}

