short pin_vitesse = 2;
short pin_cap = 4;
short pin_in1 = A2;
short pin_in2 = A1;
short pin_in3 = 6;
short pin_in4 = 8;
short pin_fna = 9;
short pin_fnb = 5;

int vitesse_moteur_1; //vitesses entre -100 (full arrière) et 100 (full avant)
int vitesse_moteur_2;


int telecommande_vitesse;
int telecommande_cap;

void setup() {
  // put your setup code here, to run once:

    pinMode(pin_in1, OUTPUT);
    pinMode(pin_in2, OUTPUT);
    pinMode(pin_in3, OUTPUT);
    pinMode(pin_in4, OUTPUT);
    pinMode(pin_fna, OUTPUT);
    pinMode(pin_fnb, OUTPUT);

    pinMode(pin_vitesse, INPUT);
    pinMode(pin_cap, INPUT);

}

void loop() {
  // put your main code here, to run repeatedly:
  telecommande_vitesse = pulseIn(pin_vitesse, HIGH);
  telecommande_cap = pulseIn(pin_cap, HIGH);


   activer_moteurs(vitesse_moteur_1, vitesse_moteur_2);
}

void activer_moteurs(int vitesse_m1, int vitesse_m2) {
    if (vitesse_m1 > 0) {
        digitalWrite(pin_in1, HIGH);
        digitalWrite(pin_in2, LOW);
    } else {
        digitalWrite(pin_in1, LOW);
        digitalWrite(pin_in2, HIGH);
    }
    if (vitesse_m2 > 0) {
        digitalWrite(pin_in3, HIGH);
        digitalWrite(pin_in4, LOW);
    } else {
        digitalWrite(pin_in3, LOW);
        digitalWrite(pin_in4, HIGH);
    }
    
    int vitesse_m1_abs = map(abs(vitesse_m1), 0, 100, 0, 255 );
    int vitesse_m2_abs = map(abs(vitesse_m2), 0, 100, 0, 255 );
    
    analogWrite(pin_fna, vitesse_m1_abs);
    analogWrite(pin_fnb, vitesse_m2_abs);
}
