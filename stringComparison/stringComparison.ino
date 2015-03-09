String str;

void setup() {
  Serial.begin(9600);
  Serial.println("Initialisation...");
}

void loop() 
{    
  if (str == "requin") {
    Serial.println("oui requin");
    str = "";
    }
    else if (str == "baleine") {
    Serial.println("oui baleine");
    str = "";
    }
}

void serialEvent () {
  if(Serial.available() > 0)
    {
        str = Serial.readStringUntil('\n');
        Serial.println(str);
    }
}
