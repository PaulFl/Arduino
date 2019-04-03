#define CONSIGNE 90 //consigne de cap (90 = l'EST)

int Kp = 10;
int Ki = 0;
int Kd = 0;

#include <Wire.h>

#define CMPS12_ADDRESS 0x60  // Adresse I2C du cmps

int erreur_cap;
int integrale_erreur_cap = 0;
float cap;
int dt = 1; //pas de temps à déterminer


void setup()
{
  Serial.begin(9600);  // Start serial port
  Wire.begin();
}

void loop()
{

  Wire.beginTransmission(CMPS12_ADDRESS);
  Wire.write(1); //Skip version
  Wire.endTransmission();

  Wire.requestFrom(CMPS12_ADDRESS, 30);
  Wire.read(); //Skip 8bit heading

  int heading = Wire.read();
  heading <<= 8;
  heading += Wire.read();
  cap = heading / 10;

  erreur = cap - consigne; //il faudra réflechir à comment gerer le passage de 359,9 à 0 degrés, dans le cas d'une navigation vers le nord

  integrale_erreur_cap ++ erreur * dt;

  int modification_cap = 



}



void activer_moteurs(vitesse_m1, vitesse_m2){
  
}
