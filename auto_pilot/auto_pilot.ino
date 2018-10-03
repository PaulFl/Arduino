//Auto Pilot
//Paul Fleury

#include "auto_pilot.h"



void setup(){
    if (DEBUG) {Serial.begin(9600);}
    
    //Initialisation centerale inertielle
    Wire.begin();
    
    //Initialisation servos
    elevon_gauche_servo.attach(ELEVONGAUCHEPIN);
    elevon_droit_servo.attach(ELEVONDROITPIN);
    moteur_esc.attach(MOTEURPIN);
}

void loop() {
    
}

void lecture_centrale_inertielle(){
    Wire.beginTransmission(ADRESSE_CMPS12);
    Wire.write(1); //Le premier octet n'est pas intéressant (version)
    Wire.endTransmission();
    
    Wire.requestFrom(ADRESSE_CMPS12, 5); //Récupération des octets 1 à 8 (1- Cap (8bits), 2 et 3 Cap (16 bits), 4 Tangage, 5 Roulis
    cap = Wire.read(); //Cap 8 bits
    Wire.read(); Wire.read();
    tangage = Wire.read();
    roulis = Wire.read();
}

void mixage_elevons(){
    
}
