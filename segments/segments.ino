char ASEG2  = 0;
char BSEG2  = 1;
char CSEG2  = 2;
char DSEG2  = 3;
char ESEG2  = 4;
char FSEG2  = 5;
char GSEG2  = 6;

boolean segments[8];

const char segmentsPin1 = 4;
const char segmentsPin2 = 3;
const char segmentsPin9 = A0;
const char segmentsPin10 = A1;
const char segmentsPin11 = A2;
const char segmentsPin12 = A3;

int number = 0;

long time;
long timeFlip;
boolean state = true;

void setup() {
  pinMode(segmentsPin1, OUTPUT);
  pinMode(segmentsPin2, OUTPUT);
  pinMode(segmentsPin9, OUTPUT);
  pinMode(segmentsPin10, OUTPUT);
  pinMode(segmentsPin11, OUTPUT);
  pinMode(segmentsPin12, OUTPUT);
  time = millis();
  timeFlip = millis();
  clearDisplay();
}

void loop() {
  if (millis() - time > 500) {
    time = millis();
    setDisplay(number);
    number++;
    if (number == 10) {
      number = 0;
    }
  }
  if (millis() - timeFlip > 2) {
    timeFlip = millis();
    updateDisplay();
  }  
}

void updateDisplay() {
  state = !state;
  digitalWrite(segmentsPin1, !state);
  digitalWrite(segmentsPin2, state);
  digitalWrite(segmentsPin9, LOW);
  digitalWrite(segmentsPin10, LOW);
  digitalWrite(segmentsPin11, LOW);
  digitalWrite(segmentsPin12, LOW);
  if (state) {
    if (segments[DSEG2]) {
      digitalWrite(segmentsPin11, HIGH);
    }
    if (segments[ESEG2]) {
      digitalWrite(segmentsPin9, HIGH);
    }
    if (segments[FSEG2]) {
      digitalWrite(segmentsPin12, HIGH);
    }
    if (segments[GSEG2]) {
      digitalWrite(segmentsPin10, HIGH);
    }
  }
  if (!state) {
    if (segments[ASEG2]) {
      digitalWrite(segmentsPin12, HIGH);
    }
    if (segments[BSEG2]) {
      digitalWrite(segmentsPin10, HIGH);
    }
    if (segments[CSEG2]) {
      digitalWrite(segmentsPin11, HIGH);
    }
  }
}

void setDisplay(int number) {
  clearDisplay();
  switch (number) {
  case 0:
    for (int i = 0; i < 6; i++) {
      segments[i] = true;
    }
    break;
  case 1:
    segments[BSEG2] = true;
    segments[CSEG2] = true;
    break;
  case 2:
    segments[ASEG2] = true;
    segments[BSEG2] = true;
    segments[DSEG2] = true;
    segments[ESEG2] = true;
    segments[GSEG2] = true;
    break;
  case 3:
    segments[ASEG2] = true;
    segments[BSEG2] = true;
    segments[CSEG2] = true;
    segments[DSEG2] = true;
    segments[GSEG2] = true;
    break;
  case 4:
    segments[BSEG2] = true;
    segments[CSEG2] = true;
    segments[FSEG2] = true;
    segments[GSEG2] = true;
    break;
  case 5:
    segments[ASEG2] = true;
    segments[CSEG2] = true;
    segments[DSEG2] = true;
    segments[FSEG2] = true;
    segments[GSEG2] = true;
    break;
  case 6:
    segments[ASEG2] = true;
    segments[CSEG2] = true;
    segments[DSEG2] = true;
    segments[ESEG2] = true;
    segments[FSEG2] = true;
    segments[GSEG2] = true;
    break;
  case 7:
    segments[ASEG2] = true;
    segments[BSEG2] = true;
    segments[CSEG2] = true;
    break;
  case 8:
    for (int i = 0; i < 7; i++) {
      segments[i] = true;
    }
    break;
  case 9:
    segments[ASEG2] = true;
    segments[BSEG2] = true;
    segments[CSEG2] = true;
    segments[DSEG2] = true;
    segments[FSEG2] = true;
    segments[GSEG2] = true;
    break;
  }
}

void clearDisplay() {
  for (int i = 0; i < 7; i++) {
    segments[i] = false;
  }
}
