#include "auto_pilot_v2.h"

void setup() {
  if (DEBUG) {
    Serial.begin(9600);
  }
  //Initialisation centerale inertielle
  Wire.begin();

  //Initialisation carte SD
  SD.begin(SDCARDCSPIN);

  //Initialisation portes logiques
  pinMode(AUTOPILOTGAUCHEPIN, OUTPUT);
  pinMode(AUTOPILOTDROITPIN, OUTPUT);

  digitalWrite(AUTOPILOTGAUCHEPIN, LOW);
  digitalWrite(AUTOPILOTDROITPIN, LOW);

  //Initialisation servos
  elevon_gauche_servo.attach(ELEVONGAUCHEPIN);
  elevon_droit_servo.attach(ELEVONDROITPIN);

  //Initialisation lecture radio
  pinMode(SWITCHAPPIN, INPUT);
}

void loop() {
  etat_AP();
  digitalWrite(AUTOPILOTGAUCHEPIN, AP_engage);
  digitalWrite(AUTOPILOTDROITPIN, AP_engage);
  piloteAuto();
  logCarteSD();
}

void lecture_centrale_inertielle() {
  Wire.beginTransmission(ADRESSE_CMPS12);
  Wire.write(1); //Le premier octet n'est pas intéressant (version)
  Wire.endTransmission();

  Wire.requestFrom(ADRESSE_CMPS12, 5); //Récupération des octets 1 à 8 (1- Cap (8bits), 2 et 3 Cap (16 bits), 4 Tangage, 5 Roulis
  cap = Wire.read(); //Cap 8 bits
  cap16 = Wire.read();
  cap16 <<= 8;
  cap16 += Wire.read();
  roulis = Wire.read(); //(0,255)
  if (roulis >= 125 && roulis <= 179) {
    roulis = roulis - 102;
  }
  roulis = roulis - 76;

  tangage = Wire.read();
  if (tangage > 127) {
    tangage -= 255;
  }
  tangage = tangage - 15;
}

void mixage_elevons() {
  //Servo: 0=min, 90=max, 180=max
  consigne_elevon_gauche = (consigne_servos_roulis / 2) - (consigne_servos_tangage / 2);
  consigne_elevon_droit = (consigne_servos_roulis / 2) + (consigne_servos_tangage / 2);

  consigne_elevon_gauche *= 0.9;
  consigne_elevon_gauche += 90;

  consigne_elevon_droit *= 0.9;
  consigne_elevon_droit += 90;
}

void actionner_servos() {
  elevon_gauche_servo.write(consigne_elevon_gauche);
  elevon_droit_servo.write(consigne_elevon_droit);
}

void asservissement_position_angulaire(int consigne_roulis, int consigne_tangage) {
  erreur_roulis = consigne_roulis - roulis;
  erreur_tangage = consigne_tangage - tangage;

  position_angulaire_PID(erreur_roulis, erreur_tangage);
}

void position_angulaire_PID(int erreur_roulis, int erreur_tangage) {
  err_roulis_k = erreur_roulis;
  err_tang_k = erreur_tangage;

  consigne_servos_roulis = int(-Kppa * ((1+Kdpa/delta_t)*err_roulis_k - (Kdpa/delta_t)*err_roulis_k_1));
  err_roulis_k_1 = err_roulis_k;

  consigne_servos_tangage = int(-Kppa * ((1+Kdpa/delta_t)*err_tang_k - (Kdpa/delta_t)*err_tang_k_1));
  err_tang_k_1 = err_tang_k;
}

void logCarteSD() {
  fichierDonnees = SD.open("log.txt", FILE_WRITE);
  if (fichierDonnees) {
    fichierDonnees.println(String(millis()) + ";" + String(roulis) + ";" + String(cap16));
    fichierDonnees.close();
  }
}

void piloteAuto() {
  lecture_centrale_inertielle();
  asservissement_position_angulaire(0, 0);
  mixage_elevons();
  actionner_servos();
}

void etat_AP() {
  int lecture = pulseIn(SWITCHAPPIN, HIGH);
  if (DEBUG) {
	Serial.println(lecture);
  }
  if (lecture > 1700){
    Kdpa = 1;
  } else {
    Kdpa = 0;
  }
  AP_engage = (lecture > 1300);
}
