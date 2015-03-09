#define SIZE_MAX     50   //taille maximale de la séquence aléatoire
#define LIMITE_TEMPS 2000 //la limite de temps pour répondre
#define VITESSE      500  //500ms de pause entre chaque note jouée

#include "pitches.h"
 
const char leds[]    = {
  10, 11, 12, 13};
const char boutons[] = {
  6, 7, 8, 9};
const char buzzer  = 5;
const int frequences[] = {
  NOTE_E4, NOTE_CS4, NOTE_A4, NOTE_E3};


 
char etats[] = {
  LOW,LOW,LOW, LOW};
char mem[]  = {
  HIGH,HIGH,HIGH, HIGH};
 
char sequence[SIZE_MAX];
char cpt = 0;
int score = 0;
 
 
void setup() {
  for(char i=0; i<4; i++) {
    pinMode(leds[i],    OUTPUT);
    pinMode(boutons[i], INPUT);
    digitalWrite(leds[i],    LOW);
    digitalWrite(boutons[i], HIGH);
  }
  pinMode(buzzer, OUTPUT);
  
  //7 segments display---

  //---------------------
 
  randomSeed(analogRead(A5));
 
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
      for(char i=0; i<cpt; i++) {
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
  sequence[cpt] = random(0,4);
  cpt++;
  score = (int)cpt - 1;
  Serial.print(score);
  if(cpt == SIZE_MAX)
    return false;
  else
    return true;
}
 
void playSequence() {
  for(char i=0; i<cpt; i++) {
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
        digitalWrite(leds[i], HIGH);
        tone(buzzer, frequences[i]);
        delay(VITESSE);
        digitalWrite(leds[i], LOW);
        noTone(buzzer);
        
        if(i==objectif) {
          return true;
        } else {
          return false; 
        }
      }
    }
  }
  return false;
}
 
void lectureBoutons() {
  for(char i=0; i<4; i++) {
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
  for(char i=0; i<4; i++) {
    for(char j=0; j<4; j++) {
      digitalWrite(leds[j], HIGH);
    }
    tone(buzzer, 1000);
    delay(500);
    for(char j=0; j<4; j++) {
      digitalWrite(leds[j], LOW);
    }
    noTone(buzzer);
    delay(500);
  }
}
 
void animationReponseBonne() {
  for(char i=0; i<4; i++) {
    digitalWrite(leds[i], HIGH);
    tone(buzzer, frequences[i]);
    delay(100);
  }
  for(char i=0; i<4; i++) {
    digitalWrite(leds[i], LOW); 
  }
  noTone(buzzer);
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

