int motor1_enablePin = 11; //pwm
int motor1_in1Pin = 13;
int motor1_in2Pin = 12;
int potentio = A0;
int value;

 
void setup()
{
  //on initialise les pins du moteur 1
  pinMode(motor1_in1Pin, OUTPUT);
  pinMode(motor1_in2Pin, OUTPUT);
  pinMode(motor1_enablePin, OUTPUT);
  Serial.begin(9600);
 
 
 
}
 
void loop()
{
 value = map(analogRead(potentio), 0, 1023, -100, 100);
 Serial.println(value);
 if (value < -20) {
  SetMotor1(map(value, -20, -100, 1, 300), true);
 }
 else if (value > 20) {
   SetMotor1(map(value, 20, 100, 1, 300), false);
 }
 else {
   digitalWrite(motor1_enablePin, LOW);
 
}
}
 
//Fonction qui set le moteur1
void SetMotor1(int speed, boolean reverse)
{
  analogWrite(motor1_enablePin, speed);
  digitalWrite(motor1_in1Pin, ! reverse);
  digitalWrite(motor1_in2Pin, reverse);
}
 

