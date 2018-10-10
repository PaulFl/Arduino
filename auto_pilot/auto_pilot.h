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

int consigne_roulis;
int consigne_tangage;
int consigne_moteur;

int erreur_roulis;
int erreur_tangage;

int consigne_elevon_gauche;
int consigne_elevon_droit;

int consigne_servos_roulis;
int consigne_servos_tangage;

int Kppa = 5; // Gain correcteur proportionnel asservissement position angulaire

//Prototypes
void lecture_centrale_inertielle();
void mixage_elevons();
void actionner_servos();
void asservissement_position_angulaire(int consigne_roulis, int consigne_tangage);



#endif
