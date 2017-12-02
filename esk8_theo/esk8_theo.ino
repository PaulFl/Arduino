#include <Servo.h>

#include <buffer.h>
#include <crc.h>
#include <datatypes.h>
#include <local_datatypes.h>
#include <printf.h>
#include <VescUart.h>

#define SERIALIO Serial

Servo moteur;

struct bldcMeasure measuredValues;

int pinBouton = 2;
int pinMoteur = 9;

float integrale = 0;

float Kp = 0.045;
float Ki = 0;
float Kd = 0.005;

float vitesseActuelle = 0;
float vitessePrecedente = 0;
float vitesseCible = 0;

bool asservir = false;

void setup() {
  Serial.begin(9600);
  moteur.attach(pinMoteur);
  attachInterrupt(digitalPinToInterrupt(pinBouton), boutonAppuye, FALLING);
  moteur.write(90);
}

void loop() {
  if (VescUartGetValue(measuredValues)) {
    vitessePrecedente = vitesseActuelle;
    vitesseActuelle = float(measuredValues.rpm);
    bool etatBouton = digitalRead(pinBouton);
    asservir = !etatBouton;
    if (asservir) {
      float ecart = vitesseCible - vitesseActuelle;
      integrale = integrale + ecart;
      float derivee = vitesseActuelle - vitessePrecedente;
      // 0 freinage max, 90 ne rien faire, 180 acceleration max
      int consigne = Kp * ecart - Kd * derivee + Ki * integrale; // +Ki * integrale + Kd *derivee
      consigne = consigne + 90;
      moteur.write(consigne);
    } else {
      moteur.write(90);
    }

  }


}

void boutonAppuye() {
  vitesseCible = vitesseActuelle;
  integrale = 0;
}
