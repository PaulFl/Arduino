#ifndef AUTOPILOT
#define AUTOPILOT

#define DEBUG 1

//Librairies
#include <Wire.h>
#include <Servo.h>

#define ADRESSE_CMPS12 0x60

//Pins
#define ELEVONGAUCHEPIN 6
#define ELEVONDROITPIN 9
#define MOTEURPIN 10

//Objets
Servo elevon_gauche_servo;
Servo elevon_droit_servo;
Servo moteur_esc;

//Variables
int roulis;
int tangage;
int cap;

//Prototypes
void lecture_centrale_inertielle();
void mixage_elevons();


#endif
