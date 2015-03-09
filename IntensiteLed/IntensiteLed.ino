//une sortie analogique sur la broche 6
const int sortieAnalogique = 11;
const int potentiometre = A1;
int valeur;

void setup()
{
    pinMode(sortieAnalogique, OUTPUT);
}

void loop()
{
  valeur = map(analogRead(potentiometre), 0, 1023, 255, 0);
    //on met un rapport cyclique de 107/255 = 42 %
    analogWrite(sortieAnalogique, valeur);
}
