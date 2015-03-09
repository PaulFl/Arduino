// Plage.ino

#define ROUGE 13
#define ORANGE 12
#define VERT 11

const int button = 2;
boolean alarm = false;
unsigned long temps;
int etat = VERT;
boolean stateAlarmLed;
String str;


void setup() {
	Serial.begin(9600);
	Serial.println("Initialisation...");
	pinMode(VERT, OUTPUT);
	pinMode(ORANGE, OUTPUT);
	pinMode(ROUGE, OUTPUT);
	pinMode(button, INPUT_PULLUP);
	leds(etat);
	attachInterrupt(button-2, buttonPressed, FALLING);
	temps = millis();
}

void loop() {
	if (alarm){
		if(millis()-temps > 500){
		    temps = millis();
		    stateAlarmLed = !stateAlarmLed;
		    digitalWrite(ROUGE, stateAlarmLed);
		}
	}
	else {
	if(str == "tempete"){
	    etat = ROUGE;
	    leds(etat);
            str = "";
	}
	else if(str == "vagues"){
	    etat = ORANGE;
	    leds(etat);
            str = "";
	}
	else if(str == "calme"){
		etat = VERT;
		leds(etat);
str = "";
	}
}
}

void leds(int led) {
	digitalWrite(VERT, LOW);
	digitalWrite(ORANGE, LOW);
	digitalWrite(ROUGE, LOW);
	digitalWrite(led, HIGH);
}

void buttonPressed () {
	alarm = !alarm;
	stateAlarmLed = LOW;
        if (alarm) {
        leds(0);
        }
        else {
	leds(etat);
        }
}

void serialEvent () {
  if(Serial.available() > 0)
    {
        str = Serial.readStringUntil('\n');
        Serial.println(str);
    }
}
