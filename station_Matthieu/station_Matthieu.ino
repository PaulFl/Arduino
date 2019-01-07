//Libraires
#include <Wire.h> //I2C
#include <DS3231.h> // Horloge
#include <Adafruit_Sensor.h> //Pour tous les autres capteurs
#include <Digital_Light_TSL2561.h>//CApteur luminosité
#include <DHT_U.h> // Capteur humidite 
#include <Adafruit_BMP280.h> //Capteru pression & temperature
#include <SPI.h> //SPI
#include <SD.h> //module carte sd


//Variables
long pression;
int temperature;
int humidite;
int luminosite;
int son;

int annee;
int mois;
int jour;
int heure;
int minute;
int seconde;

//Branchements
int pinSon = A0; //analogique
int pinHumidite = 2;

const int pinCSCarteSD = 10; //SPI

//Definition objets
//Humidité
DHT_Unified capteurHumidite(pinHumidite, DHT11);
//Pression & Temperature
Adafruit_BMP280 capteurPressionTemperature;
//Horloge
RTClib horloge;

//Pas besoin de derinition pour le son car entreee analogique


//Capteurs
sensors_event_t event; //Variable utiliseee par Adafruit_Unified pour gerer tous les capteurs en meme temps, (STRUCT structure en C)


void setup() {
  //Initalisation port sérire
  Serial.begin(9600);

  //Initialisation horloge
  Wire.begin();

  //Initialisation capteur luminosité
  TSL2561.init();

  //Initialisation capteur humidté
  capteurHumidite.begin();

  //Initialisation capteur pression & Temperature
  capteurPressionTemperature.begin();

  //Initialisation carteSD
  SD.begin(pinCSCarteSD);

}

void loop() {
  mesurerSon();
  Serial.print("Son: ");
  Serial.println(son);
  
  //mesurerHumidite();
  //Serial.println(humidite);
  
  mesurerLuminosite();
  Serial.print("Luminosité: ");
  Serial.println(luminosite);
  
  mesurerPressionTemperature();
  Serial.print("Pression: ");
  Serial.println(pression);
  
  Serial.print("Température: ");
  Serial.println(temperature);
  
  recupererDate();

  ecrireCarteSD();


  delay(10000); //~10 secondes entre les mesures
}

void mesurerSon() {
  son = analogRead(pinSon); //pour l'instant rencoie une valeur codee sur 10 bits cest a dire entre 0 et 1023
}

void mesurerHumidite() {
  capteurHumidite.humidity().getEvent(&event);
  humidite = event.relative_humidity;
}

void mesurerLuminosite() {
  luminosite = TSL2561.readVisibleLux();
}

void mesurerPressionTemperature() {
  temperature = capteurPressionTemperature.readTemperature();
  pression = capteurPressionTemperature.readPressure();
}

void recupererDate() {
  DateTime maintenant = horloge.now();
  annee = maintenant.year();
  mois = maintenant.month();
  jour = maintenant.day();
  heure = maintenant.hour();
  minute = maintenant.minute();
  seconde = maintenant.second();
}

void ecrireCarteSD() {
  String ligne = ""; //(+= rajoute a la fin de la ligen (equivalent a crire ligne = ligne +"blablea"), ";" pour separer les valeurs
  ligne += String(annee); //Date
  ligne += ";";
  ligne += String(mois);
  ligne += ";";
  ligne += String(jour);
  ligne += ";";
  ligne += String(heure);
  ligne += ";";
  ligne += String(minute);
  ligne += ";";
  ligne += String(seconde);
  ligne += ";";

  ligne += String(temperature);
  ligne += ";";
  ligne += String(pression);
  ligne += ";";
  ligne += String(luminosite);
  ligne += ";";
  ligne += String(humidite);
  ligne += ";";
  ligne += String(son);
  ligne += ";";

  File fichierDonnees = SD.open("donnees.txt", FILE_WRITE);
  fichierDonnees.println(ligne);
  fichierDonnees.close();
}
