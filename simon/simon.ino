#include <EEPROM.h>
#include "functions.h"

#define SIZE_MAX     99   //taille maximale de la séquence aléatoire
#define LIMITE_TEMPS 2000 //la limite de temps pour répondre
#define VITESSE      500  //500ms de pause entre chaque note jouée

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


long BetterRandom( long howsmall, long howbig )
{
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
      playSequence();    
      
      mancheOK = true;
      for(short i=0; i<cpt; i++) {
        if(!checkEtape(i)) {
          mancheOK = false;
          break;
        }
      }
      if(!mancheOK) {
        nbEssai++;
        animationReponseFausse();
      } 
      else {
        animationReponseBonne(); 
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
 
void playSequence() {
  for(short i=0; i<cpt; i++) {
    digitalWrite(leds[sequence[i]], HIGH);
    tone(buzzer, frequences[sequence[i]]);
    delay(VITESSE);
    digitalWrite(leds[sequence[i]], LOW); 
    noTone(buzzer);
    delay(VITESSE);
  }
}
 
boolean checkEtape(char etape) {
  long temps = millis();
  char objectif = sequence[etape];
 
  while((millis() - temps) < LIMITE_TEMPS) {
    lectureBoutons();
    for(int i=0; i<4; i++) {
      if(etats[i]) {
	if(i != objectif) {
		return false;
	}
	else if(i == objectif) {
        digitalWrite(leds[i], HIGH);
        tone(buzzer, frequences[i]);
        delay(VITESSE);
        digitalWrite(leds[i], LOW);
        noTone(buzzer);
	return true;
	}
	} 
        }
  }
  return false;
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

void animationReponseFausse() {
   digitalWrite(leds[sequence[cpt-1]], HIGH); 
    tone(buzzer, NOTE_A2);
    delay(750);
    digitalWrite(leds[sequence[cpt-1]], LOW);
    noTone(buzzer);
    delay(500);
}
 
void animationReponseBonne() {
	delay(500);
}
 
void ecranFin(boolean perdu) {
  if(perdu) {
    while(1) {
      //joue la séquence si on lit un état bas
      if(!digitalRead(boutons[0]) || !digitalRead(boutons[1]) || !digitalRead(boutons[2]) || !digitalRead(boutons[3])) {
        playSequence(); 
      }
    }
  }
}

