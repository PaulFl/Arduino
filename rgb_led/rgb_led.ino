#define SPEED 50
short red = 3;
short green = 5;
short blue = 6;

int newBlue = 0;
int newRed = 0;
int newGreen = 0;

int blueValue = 0;
int greenValue = 0;
int redValue = 0;

long lastChange;

short color;

void setup() {

  pinMode(red, OUTPUT);
  pinMode(blue, OUTPUT);
  pinMode(green, OUTPUT);

  lastChange = millis();
}

void loop() {
  analogWrite(blue, blueValue);
  analogWrite(green, greenValue);
  analogWrite(red, redValue);

  if (blueValue == newBlue && redValue == newRed && greenValue == newGreen) {
    color = random(6);
    switch (color) {
      case 0:
        newBlue = 255;
        newGreen = 0;
        newRed = 0;
        break;
      case 1:
        newBlue = 0;
        newGreen = 255;
        newRed = 0;
        break;
      case 2:
        newBlue = 0;
        newGreen = 0;
        newRed = 255;
        break;
      case 3:
        newBlue = 255;
        newGreen = 255;
        newRed = 0;
        break;
      case 4:
        newBlue = 0;
        newGreen = 255;
        newRed = 255;
        break;
      case 5:
        newBlue = 255;
        newGreen = 0;
        newRed = 255;
        break;
    }
  } else if (millis() - lastChange > SPEED) {
    lastChange = millis();
    if (blueValue > newBlue) {
      blueValue--;
    } else if ( blueValue < newBlue) {
      blueValue++;
    }
    if (greenValue > newGreen) {
      greenValue--;
    } else if (greenValue < newGreen) {
      greenValue++;
    }
    if (redValue > newRed) {
      redValue--;
    } else if (redValue < newRed) {
      redValue++;
    }
  }
}



