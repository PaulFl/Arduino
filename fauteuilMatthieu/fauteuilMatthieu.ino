#include <Servo.h>

int milieuServo = 98; //valeuur pour que le servo ne bouge pas 

short moteurGauchePin = 2; // prise
short moteurDroitPin = 3; // prise

Servo moteurGauche;
Servo moteurDroit;

short joystickXPin = A0; //prise
short joystickYPin = A1; //prise

int valeurJoystickX = 0; //valeur lue du joystick pour la direction (gauche droite)
int valeurJoystickY = 0; //valeur lue du joystick pour la vitesse (avant arriere)

int vitesseGauche = milieuServo; //vitesse cible calculée du moteur gauche 
int vitesseDroite = milieuServo; //vitesse cible calculée du moteur droit

int vitesseActuelleGauche = milieuServo;
int vitesseActuelleDroite = milieuServo;

int vitesseLente = 250; //par rapport au milieu joystick
int vitesseRapide = 488; //par rapport au milieu joysitck

int vitesseMoteurLente = 25;//par rapport a 90
int vitesseMoteurRapide = 50; //par rapport a 90

int vitesseRotationLente = 250;
int vitesseRotationRapide = 488;

int vitesseRotationMoteurLente = 20;
int vitesseRotationMoteurRapide = 30;


int milieuJoystick = 512; //centre du joystick

void setup(){
  Serial.begin(9600);

  moteurGauche.attach(moteurGauchePin); //associer l'objet moteur gauche à sa prise
  moteurDroit.attach(moteurDroitPin); //associer l'objet moteur droit a sa prise
}

void loop() {
  calculVitesses();
  changementVitesseMoteur();
  delay(20);
}

void calculVitesses() {
  valeurJoystickX = analogRead(joystickXPin);
  valeurJoystickY = analogRead(joystickYPin);

  if (valeurJoystickY > milieuJoystick + vitesseRapide){
    vitesseGauche = milieuServo + vitesseMoteurRapide;
    vitesseDroite = milieuServo + vitesseMoteurRapide;
  } else if (valeurJoystickY < milieuJoystick - vitesseRapide){
    vitesseGauche = milieuServo - vitesseMoteurRapide;
    vitesseDroite = milieuServo - vitesseMoteurRapide; 
  } else if (valeurJoystickY > milieuJoystick + vitesseLente){
    vitesseGauche = milieuServo + vitesseMoteurLente;
    vitesseDroite = milieuServo + vitesseMoteurLente; 
  } else if (valeurJoystickY < milieuJoystick - vitesseLente){
    vitesseGauche = milieuServo - vitesseMoteurLente;
    vitesseDroite = milieuServo - vitesseMoteurLente; 
  } else {
    vitesseGauche = milieuServo;
    vitesseDroite = milieuServo;
  }

  

  if (valeurJoystickX > milieuJoystick + vitesseRotationRapide){
    vitesseGauche = vitesseGauche - vitesseRotationMoteurRapide;
    vitesseDroite = vitesseDroite + vitesseRotationMoteurRapide;
  } 
  else if (valeurJoystickX < milieuJoystick - vitesseRotationRapide){
    vitesseGauche = vitesseGauche + vitesseRotationMoteurRapide;
    vitesseDroite = vitesseDroite - vitesseRotationMoteurRapide; 
  } 
  else if (valeurJoystickX > milieuJoystick + vitesseRotationLente){
    vitesseGauche = vitesseGauche - vitesseRotationMoteurLente;
    vitesseDroite = vitesseDroite + vitesseRotationMoteurLente;
  } 
  else if (valeurJoystickX < milieuJoystick - vitesseRotationLente){
    vitesseGauche = vitesseGauche + vitesseRotationMoteurLente;
    vitesseDroite = vitesseDroite - vitesseRotationMoteurLente; 
  }

  
}

void changementVitesseMoteur() {
  if (vitesseGauche > vitesseActuelleGauche) { //Calcul
    vitesseActuelleGauche ++;
  } else if (vitesseGauche < vitesseActuelleGauche) {
    vitesseActuelleGauche --;
  }
  if (vitesseDroite > vitesseActuelleDroite) {
    vitesseActuelleDroite ++;
  } else if (vitesseDroite < vitesseActuelleDroite) {
    vitesseActuelleDroite --;
  }
  moteurGauche.write(vitesseActuelleGauche); //envoie de la commande au servo
  moteurDroit.write(vitesseActuelleDroite);

  Serial.print(vitesseActuelleGauche);
  Serial.print("\t");
  Serial.println(vitesseActuelleDroite);
}
