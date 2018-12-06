//Auto Pilot
//Paul Fleury
#include "robot_ball_balance_v1.h"

#include "auto_pilot.h"

//Toutes les valeurs sont comprises entre -100 et 100



void setup() {
  if (DEBUG) {
    Serial.begin(9600);
  }

  //Initialisation centerale inertielle
  Wire.begin();

  //Initialisation carte SD
  SD.begin(SDCARDCSPIN);

  //Initialisation servos
  elevon_gauche_servo.attach(ELEVONGAUCHEPIN);
  elevon_droit_servo.attach(ELEVONDROITPIN);
  moteur_esc.attach(MOTEURPIN);
  initCI();
}

void loop() {
  lireTransmetteur();
  recopieMoteur();
  if (valeursRadio[4] < 1500) {
    recopieRadio();
  } else {
    piloteAuto();
  }
  //logCarteSD();
}

void lecture_centrale_inertielle() {
  Wire.beginTransmission(ADRESSE_CMPS12);
  Wire.write(1); //Le premier octet n'est pas intéressant (version)
  Wire.endTransmission();

  Wire.requestFrom(ADRESSE_CMPS12, 5); //Récupération des octets 1 à 8 (1- Cap (8bits), 2 et 3 Cap (16 bits), 4 Tangage, 5 Roulis
  cap = Wire.read(); //Cap 8 bits
  Wire.read(); Wire.read();
  tangage = Wire.read(); //(0,255)
  if (tangage > 127) {
    tangage -= 255;
  }

  roulis = Wire.read();
  if (roulis > 127) {
    roulis -= 255;
  }
}

void mixage_elevons() {
  //Servo: 0=min, 90=max, 180=max
  consigne_elevon_gauche = (consigne_servos_roulis / 2) - (consigne_servos_tangage / 2);
  consigne_elevon_droit = (consigne_servos_roulis / 2) + (consigne_servos_tangage / 2);

  consigne_elevon_gauche *= 0.9;
  consigne_elevon_gauche += 90;
  if (DEBUG) {
    Serial.println(consigne_elevon_gauche);
  }

  consigne_elevon_droit *= 0.9;
  consigne_elevon_droit += 90;
}

void actionner_servos() {
  if (consigne_elevon_gauche > 130) consigne_elevon_gauche = 130;
  if (consigne_elevon_gauche < 25) consigne_elevon_gauche = 25;
  if (consigne_elevon_droit > 130) consigne_elevon_droit = 130;
  if (consigne_elevon_droit < 24) consigne_elevon_droit = 25;
  elevon_gauche_servo.write(consigne_elevon_gauche);
  elevon_droit_servo.write(consigne_elevon_droit);
}

void asservissement_position_angulaire(int consigne_roulis, int consigne_tangage) {
  erreur_roulis = consigne_roulis - roulis;
  erreur_tangage = consigne_tangage - tangage;

  consigne_servos_roulis = -Kppa * erreur_roulis;
  consigne_servos_tangage = -Kppa * erreur_tangage;
}

void logCarteSD() {
  fichierDonnees = SD.open("log.txt", FILE_WRITE);
  if (fichierDonnees) {
    fichierDonnees.println(String(millis()) + ";" + String(roulis));
    fichierDonnees.close();
  }
}

void lireTransmetteur() {
  for (int voie = 1; voie <= NBVOIESRADIO; voie++) {
    valeursRadio[voie - 1] = ppm.rawChannelValue(voie);
    if (DEBUG) {
      Serial.print(String(valeursRadio[voie - 1]) + " ");
    }
  }
  Serial.println();
}

void recopieRadio() {
  //  consigne_elevon_gauche = map(valeursRadio[0], 1000, 2000, 45, 183);
  //  consigne_elevon_droit = map(valeursRadio[1], 1000, 2000, 45, 183);
  consigne_elevon_gauche = map(valeursRadio[0], 1000, 2000, 25, 130);
  consigne_elevon_droit = map(valeursRadio[1], 1000, 2000, 25 , 130);
  actionner_servos();
}

void recopieMoteur() {
  moteur_esc.write(map(valeursRadio[2], 1000, 2000, 45, 183));
}

void piloteAuto() {
  readCI();
  roulis = x.angleCentered;
  tangage = y.angleCentered;
  //lecture_centrale_inertielle();
  asservissement_position_angulaire(0, 0);
  mixage_elevons();
  actionner_servos();
}
