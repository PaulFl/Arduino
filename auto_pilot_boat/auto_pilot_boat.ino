#define CONSIGNE_CAP 90 //consigne de cap (90 = l'EST)
#define CONSIGNE_VITESSE 50

#define INVERSER_M1 0
#define INVERSER_M2 0

int Kp = 10;
int Ki = 0;
int Kd = 0;

#include <Wire.h> //Librairie pour le cmps
#include "PPMReader.h" //Pour lire les données du récepteur de la télécommande

#define CMPS12_ADDRESS 0x60  // Adresse I2C du cmps


//Pins
short pin_telecommande = 3;
short pin_in1 = 10;
short pin_in2 = 9;
short pin_in3 = 6;
short pin_in4 = 8;
short pin_fna = 11;
short pin_fnb = 5;


int nb_voies_telecommande = 6;
PPMReader ppm(pin_telecommande, nb_voies_telecommande);


int erreur_cap;
int integrale_erreur_cap = 0;
float cap;
int dt = 10; //pas de temps à déterminer
float u_k;
float u_k_1;
float e_k;
float e_k_1;
float e_k_2;

int vitesse_moteur_1; //vitesses entre -100 (full arrière) et 100 (full avant)
int vitesse_moteur_2;

bool asservir = false;

int telecommande_switch = false;
int telecommande_vitesse;
int telecommande_cap;

void setup()
{
    Serial.begin(9600);  // Start serial port
    Wire.begin();
    
    pinMode(pin_in1, OUTPUT);
    pinMode(pin_in2, OUTPUT);
    pinMode(pin_in3, OUTPUT);
    pinMode(pin_in4, OUTPUT);
    pinMode(pin_fna, OUTPUT);
    pinMode(pin_fnb, OUTPUT);
}

void loop()
{
    lire_telecommande();
    
    if (telecommande_switch > 1500) {
        asservir = true;
    }
    
    Wire.beginTransmission(CMPS12_ADDRESS);
    Wire.write(1); //Skip version
    Wire.endTransmission();
    
    Wire.requestFrom(CMPS12_ADDRESS, 30);
    Wire.read(); //Skip 8bit heading
    
    int heading = Wire.read();
    heading <<= 8;
    heading += Wire.read();
    cap = heading / 10;
    
    e_k = cap - CONSIGNE_CAP; //il faudra réflechir à comment gerer le passage de 359,9 à 0 degrés, dans le cas d'une navigation vers le nord
    
    u_k = u_k_1 + Kp * (e_k * (1 + Ki * dt + Kd / dt) + e_k_1 * (-1 - 2 * Kd / dt) + e_k_2 * (Kd / dt)); //PID
    
    u_k_1 = u_k;
    e_k_2 = e_k_1;
    e_k_1 = e_k;
    
    delay(dt);
    //SI u_k positif -> vers la droite, si u_k négatif -> vers la gauche
    
    if (asservir) {
        vitesse_moteur_1 = CONSIGNE_VITESSE + u_k;
        vitesse_moteur_2 = CONSIGNE_VITESSE - u_k;
    } else {
        int vitesse = (telecommande_vitesse - 1500) / 5;
        int difference_vitesse = (telecommande_cap - 1500) / 5;
        
        vitesse_moteur_1 = vitesse + difference_vitesse;
        vitesse_moteur_2 = vitesse - difference_vitesse;
    }
    activer_moteurs(vitesse_moteur_1, vitesse_moteur_2);
    
    
}

void lire_telecommande() {
    telecommande_vitesse = ppm.rawChannelValue(1);
    telecommande_cap = ppm.rawChannelValue(4);
    telecommande_switch = ppm.rawChannelValue(5);
}



void activer_moteurs(int vitesse_m1, int vitesse_m2) {
    if (vitesse_moteur_1 > 0) {
        digitalWrite(pin_in1, HIGH);
        digitalWrite(pin_in2, LOW);
    } else {
        digitalWrite(pin_in1, LOW);
        digitalWrite(pin_in2, HIGH);
    }
    if (vitesse_moteur_2 > 0) {
        digitalWrite(pin_in3, HIGH);
        digitalWrite(pin_in4, LOW);
    } else {
        digitalWrite(pin_in3, LOW);
        digitalWrite(pin_in4, HIGH);
    }
    
    int vitesse_m1_abs = map(abs(vitesse_moteur_1), 0, 100, 0, 255 );
    int vitesse_m2_abs = map(abs(vitesse_moteur_2), 0, 100, 0, 255 );
    
    analogWrite(pin_fna, vitesse_m1_abs);
    analogWrite(pin_fnb, vitesse_m2_abs);
}
