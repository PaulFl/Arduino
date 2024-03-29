#ifndef AUTOPILOT
#define AUTOPILOT

#define DEBUG 1

//Librairies
#include <Wire.h>
#include <Servo.h>
#include <SPI.h>
#include "SdFat.h"

#define ADRESSE_CMPS12 0x60

//Pins
#define AUTOPILOTGAUCHEPIN A0
#define AUTOPILOTDROITPIN A1
#define ELEVONGAUCHEPIN 6
#define ELEVONDROITPIN 9
#define SWITCHAPPIN 3
#define SDCARDCSPIN 4



//Variables

bool AP_engage = false;

int roulis;
int tangage;
int cap; //codé sur 8 bits
int cap16; // codé sur 16 bits

int consigne_roulis;
int consigne_tangage;
int consigne_moteur;

int erreur_roulis;
int erreur_tangage;

int consigne_elevon_gauche;
int consigne_elevon_droit;

int consigne_servos_roulis;
int consigne_servos_tangage;

float Kppa = 3; // Gain correcteur proportionnel asservissement position angulaire
float Kdpa = 0;
float delta_t = 0.1;

//Objets
Servo elevon_gauche_servo;
Servo elevon_droit_servo;

SdFat SD;
File fichierDonnees;

String logStr;

//PID
//erreurs
float err_roulis_k = 0;
float err_roulis_k_1 = 0;
float err_tang_k = 0;
float err_tang_k_1 = 0;



//Prototypes
void lecture_centrale_inertielle();
void mixage_elevons();
void actionner_servos();
void asservissement_position_angulaire(int consigne_roulis, int consigne_tangage);
void log_carte_SD();
void pilote_auto();
void etat_AP();
void position_angulaire_PID(int erreur_roulis, int erreur_tangage);



#endif
