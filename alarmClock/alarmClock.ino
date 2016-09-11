/*!!!!!!!!! in IRremonteInt.h, set 
 
 // Arduino Duemilanove, Diecimila, LilyPad, Mini, Fio, Nano, etc
#else
  #define IR_USE_TIMER1   // tx = pin 9
  //#define IR_USE_TIMER2     // tx = pin 3

#endif */


#include "segments.h"
#include "pins.h"
#include "pitches.h"
#include "functions.h"
#include "irValues.h"

#include "Time.h"
#include "IRremote.h"

#define DEBUG 0
#define REFRESHSPEED 500
#define BLINKSPEED 500
#define TIMEOUT 30000
#define SETTINGSDELAY 1500
#define DEBOUNCEDELAY 100
#define IRDELAY 100
#define ANALOGLIMIT 1010

unsigned long flipmicros;
unsigned long blinkmillis;
unsigned long timeoutDisplay;
unsigned long pastIRReading;

boolean blinkState = true;
boolean segments[6][8];
boolean blink[6] = {false};
boolean displaySeconds = true;
boolean lastAlarmSwitchState;
byte onSegment = 0;
byte actualDisplay;

byte pastSeconds;
byte pastDate;

int buttonDuration;

//boolean irRemoteState = false;
//boolean lastIrRemoteState = false;
boolean irClean = false;

int irMainValue;

IRrecv irrecv(iRReceiver);
decode_results results;

int alarmHour = 7;
int alarmMinute = 0;
boolean alarmEnabled = false;

void setup() {
  if (DEBUG) {
    Serial.begin(9600);
  }

  //Init pins
  pinMode(button, INPUT_PULLUP);

  //Init segments pins
  for (int i = 0; i < 6; i++) {
    pinMode(cathodeSegments[i], OUTPUT);
    digitalWrite(cathodeSegments[i], HIGH);
  }
  for (int i = 0; i < 8; i++) {
    pinMode(anodeSegments[i], OUTPUT);
    digitalWrite(anodeSegments[i], LOW);
  }

  //Start IR Receiver
  irrecv.enableIRIn();

  //Play init sound
  tone(speaker, NOTE_A3, 500);
  //Set time and date
  setTime(0, 0, 0, 0, 1, 2015);
  setTimeUser();
  setDateUser();
  pastSeconds = 60;
  pastDate = 0;
  actualDisplay = TIME;
  pinMode(speaker, OUTPUT);
  digitalWrite(speaker, LOW);

  pinMode(led, OUTPUT);
  pinMode(alarmSwitch, INPUT_PULLUP);
  lastAlarmSwitchState = digitalRead(alarmSwitch);
}

void loop() {
  if (hour() == alarmHour && minute() == alarmMinute && second() == 0 && alarmEnabled) {
    alarm();
  }
  flip();
  buttonDuration = 0;
  switch (actualDisplay) {
    case TIME:
      if (pastSeconds != second()) {
        pastSeconds = second();
        displayTime(displaySeconds);
      }
      buttonDuration = readButton();
      irMainValue = readIR();
      switch (irMainValue) {
        case ENTER:
          actualDisplay = DATE;
          displayDate();
          timeoutDisplay = millis();
          break;
        case SETTINGS:
          setTimeUser();
          break;
        case LIST:
          displaySeconds = !displaySeconds;
          displayTime(displaySeconds);
          break;
        default:
          break;
      }
      if (buttonDuration > SETTINGSDELAY) setTimeUser();
      else if (buttonDuration) {
        actualDisplay = DATE;
        displayDate();
        timeoutDisplay = millis();
      }
      break;
    case DATE:
      if (pastDate != day()) {
        pastDate = day();
        displayDate();
      }
      buttonDuration = readButton();
      irMainValue = readIR();
      switch (irMainValue) {
        case ENTER:
          actualDisplay = ALARM;
          displayAlarm();
          timeoutDisplay = millis();
          break;
        case SETTINGS:
          setDateUser();
          break;
        default:
          break;
      }
      if (buttonDuration > SETTINGSDELAY) setDateUser();
      else if (millis() - timeoutDisplay > TIMEOUT) {
        actualDisplay = TIME;
        displayTime(displaySeconds);
      }
      else if (buttonDuration) {
        actualDisplay = ALARM;
        displayAlarm();
        timeoutDisplay = millis();
      }
      break;
    case ALARM:
      buttonDuration = readButton();
      irMainValue = readIR();
      switch (irMainValue) {
        case ENTER:
          actualDisplay = TIME;
          displayTime(displaySeconds);
          break;
        case SETTINGS:
          setAlarmUser();
          break;
        case LIST:
          alarmEnabled = !alarmEnabled;
          //if (alarmEnabled) {
          //  alarmID = Alarm.alarmRepeat(alarmHour, alarmMinute, 0, alarm);
          //} else {
          //  Alarm.free(alarmID);
          //}
          break;
        default:
          break;
      }
      if (buttonDuration > SETTINGSDELAY) setAlarmUser();
      else if (buttonDuration || millis() - timeoutDisplay > TIMEOUT) {
        actualDisplay = TIME;
        displayTime(displaySeconds);
      }
      break;
  }
}

void displayAlarm() {
  byte array[6];
  boolean separators[6] = {0, 0, 0, 0, 0, 0};
  array[0] = ACHAR;
  array[1] = LCHAR;
  array[2] = alarmHour / 10;
  array[3] = alarmHour % 10;
  array[4] = alarmMinute / 10;
  array[5] = alarmMinute % 10;
  separators[5] = true;
  separators[1] = true;
  display(array);
  setSeparators(separators);
}

void displayTime(boolean displaySec) {
  byte array[6];
  boolean separators[6] = {0, 0, 0, 0, 0, 0};
  if (displaySec) {
    if (hour() < 10) {
      array[0] = OFFSEG;
    }
    else {
      array[0] = hour() / 10;
    }
    array[1] = hour() % 10;
    array[2] = minute() / 10;
    array[3] = minute() % 10;
    array[4] = second() / 10;
    array[5] = second() % 10;
    separators[4] = true;
    separators[5] = true;
  }
  else {
    array[0] = OFFSEG;
    array[1] = OFFSEG;
    if (hour() < 10) {
      array[2] = OFFSEG;
    }
    else {
      array[2] = hour() / 10;
    }
    array[3] = hour() % 10;
    array[4] = minute() / 10;
    array[5] = minute() % 10;
    separators[5] = true;
  }
  display(array);
  setSeparators(separators);
}

void displayDate() {
  byte array[6];
  if (day() < 10) {
    array[0] = OFFSEG;
  }
  else {
    array[0] = day() / 10;
  }
  array[1] = day() % 10;
  if (month() < 10) {
    array[2] = OFFSEG;
  }
  else {
    array[2] = month() / 10;
  }
  array[3] = month() % 10;
  array[4] = (year() / 10) % 10;
  array[5] = year() % 10;
  display(array);
  boolean separators[6] = {0, 1, 0, 1, 0, 0};
  setSeparators(separators);
}

void display(byte text[6]) {
  for (int i = 0; i < 6; i++) {
    for (int j = 0; j < 7; j++) {
      segments[i][j] = chars[text[i]][j];
    }
  }

}

void setSeparators(boolean separators[6]) {
  for (int i = 0; i < 6; i++) {
    segments[i][7] = separators[i];
  }
}

void flip() {
  if (micros() - flipmicros > REFRESHSPEED) {
    if (lastAlarmSwitchState != digitalRead(alarmSwitch)) {
      alarmEnabled = !alarmEnabled;
      lastAlarmSwitchState = digitalRead(alarmSwitch);
    }
    digitalWrite(cathodeSegments[onSegment], LOW);
    onSegment++;
    if (onSegment == 6) {
      onSegment = 0;
    }
    if ((millis() - blinkmillis > BLINKSPEED) && blinkState) {
      blinkState = false;
      blinkmillis = millis();
    }
    else if ((millis() - blinkmillis > BLINKSPEED / 5) && !blinkState) {
      blinkState = true;
      blinkmillis = millis();
    }
    for (int i = 0; i < 8; i++) {
      digitalWrite(anodeSegments[i], segments[onSegment][i]);
    }
    if (blink[onSegment]) {
      digitalWrite(cathodeSegments[onSegment], blinkState);
    }
    else {
      digitalWrite(cathodeSegments[onSegment], HIGH);
    }
    if (alarmEnabled) digitalWrite(led, HIGH);
    else digitalWrite(led, LOW);
    flipmicros = micros();
  }
}

void setTimeUser() {
  unsigned long lastAction = millis();
  boolean done = false;
  boolean edit = false;
  int selection = 0;
  int pastAnalog = analogRead(analogInput);
  while (!done) {
    displayTime(true); //if pastSeconds != seconds() or minutes or hours?...
    flip();
    for (int i = 0; i < 6; i++) {
      if ((i == (2 * selection)) || (i == ((2 * selection) + 1))) {
        blink[i] = true;
      }
      else {
        blink[i] = false;
      }
    }
    if ((analogRead(analogInput) > (pastAnalog + 5)) || (analogRead(analogInput) < (pastAnalog - 5))) {
      edit = true;
      if (DEBUG) {
      Serial.println("edit true");
      Serial.print(pastAnalog);
      Serial.print(analogRead(analogInput));
      }
      lastAction = millis();
      pastAnalog = analogRead(analogInput);
    }
    if (edit) {
      switch (selection) {
        case 0:
          setTime(map(analogRead(analogInput), 0, ANALOGLIMIT, 0, 23), minute(), second(), day(), month(), year());
          break;
        case 1:
          setTime(hour(), map(analogRead(analogInput), 0, ANALOGLIMIT, 0, 59), second(), day(), month(), year());
          break;
        case 2:
          setTime(hour(), minute(), map(analogRead(analogInput), 0, ANALOGLIMIT, 0, 59), day(), month(), year());
          break;
        default:
          break;
      }
    }
    if (millis() - lastAction > TIMEOUT) {
      done = true;
    }
    if (readButton()) {
      selection ++;
      pastAnalog = analogRead(analogInput);
      edit = false;
      lastAction = millis();
      if (selection == 3) {
        done = true;
      }
    }
    int irValue = readIR();
    switch (irValue) {
      case RIGHT:
        selection++;
        pastAnalog = analogRead(analogInput);
        edit = false;
        lastAction = millis();
        if (selection == 3) selection = 2;
        break;
      case LEFT:
        selection--;
        pastAnalog = analogRead(analogInput);
        edit = false;
        lastAction = millis();
        if (selection == -1) selection = 0;
        break;
      case ENTER:
        pastAnalog = analogRead(analogInput);
        edit = false;
        lastAction = millis();
        done = true;
        break;
      case UP:
        lastAction = millis();
        switch (selection) {
          case 0:
            setTime(hour() + 1, minute(), second(), day(), month(), year());
            break;
          case 1:
            setTime(hour(), minute() + 1, second(), day(), month(), year());
            break;
          case 2:
            setTime(hour(), minute(), second() + 1, day(), month(), year());
            break;
          default:
            break;
        }
        break;
      case DOWN:
        lastAction = millis();
        switch (selection) {
          case 0:
            if (hour() == 0) setTime(23, minute(), second(), day(), month(), year());
            else setTime(hour() - 1, minute(), second(), day(), month(), year());
            break;
          case 1:
            if (minute() == 0) setTime(hour(), 59, second(), day(), month(), year());
            else setTime(hour(), minute() - 1, second(), day(), month(), year());
            break;
          case 2:
            if (second() == 0) setTime(hour(), minute(), 59, day(), month(), year());
            else setTime(hour(), minute(), second() - 1, day(), month(), year());
            break;
          default:
            break;
        }
        break;
      default:
        break;
    }
    if (done) {
      for (int i = 0; i < 6; i++) {
        blink[i] = false;
      }
    }
  }
}

void setAlarmUser() {
  unsigned long lastAction = millis();
  boolean done = false;
  boolean edit = false;
  byte selection = 1;
  int pastAnalog = analogRead(analogInput);
  while (!done) {
    displayAlarm();
    flip();
    for (int i = 0; i < 6; i++) {
      if ((i == (2 * selection)) || (i == ((2 * selection) + 1))) {
        blink[i] = true;
      }
      else {
        blink[i] = false;
      }
    }
    if ((analogRead(analogInput) > (pastAnalog + 5)) || (analogRead(analogInput) < (pastAnalog - 5))) {
      edit = true;
      lastAction = millis();
      pastAnalog = analogRead(analogInput);
    }
    if (edit) {
      switch (selection) {
        case 1:
          alarmHour = map(analogRead(analogInput), 0, ANALOGLIMIT, 0, 23);
          break;
        case 2:
          alarmMinute = map(analogRead(analogInput), 0, ANALOGLIMIT, 0, 59);
          break;
        default:
          break;
      }
    }
    if (millis() - lastAction > TIMEOUT) {
      done = true;
    }
    if (readButton()) {
      selection ++;
      pastAnalog = analogRead(analogInput);
      edit = false;
      lastAction = millis();
      if (selection == 3) {
        done = true;
      }
    }
    int irValue = readIR();
    switch (irValue) {
      case RIGHT:
        selection++;
        pastAnalog = analogRead(analogInput);
        edit = false;
        lastAction = millis();
        if (selection == 3) selection = 2;
        break;
      case LEFT:
        selection--;
        pastAnalog = analogRead(analogInput);
        edit = false;
        lastAction = millis();
        if (selection == 0) selection = 1;
        break;
      case ENTER:
        pastAnalog = analogRead(analogInput);
        edit = false;
        lastAction = millis();
        done = true;
        break;
      case UP:
        lastAction = millis();
        switch (selection) {
          case 1:
            alarmHour++;
            if (alarmHour == 24) alarmHour = 0;
            break;
          case 2:
            alarmMinute++;
            if (alarmMinute == 60) alarmMinute = 0;
            break;
          default:
            break;
        }
        break;
      case DOWN:
        lastAction = millis();
        switch (selection) {
          case 1:
            alarmHour--;
            if (alarmHour == -1) alarmHour = 23;
            break;
          case 2:
            alarmMinute--;
            if (alarmMinute == -1) alarmMinute = 59;
            break;
          default:
            break;
        }
        break;
      default:
        break;
    }
    if (done) {
      for (int i = 0; i < 6; i++) {
        blink[i] = false;
      }
    }
  }
}

void setDateUser() {
  unsigned long lastAction = millis();
  boolean done = false;
  boolean edit = false;
  byte selection = 0;
  int pastAnalog = analogRead(analogInput);
  while (!done) {
    displayDate();
    flip();
    for (int i = 0; i < 6; i++) {
      if ((i == (2 * selection)) || (i == ((2 * selection) + 1))) {
        blink[i] = true;
      }
      else {
        blink[i] = false;
      }
    }
    if ((analogRead(analogInput) > (pastAnalog + 5)) || (analogRead(analogInput) < (pastAnalog - 5))) {
      edit = true;
      lastAction = millis();
      pastAnalog = analogRead(analogInput);
    }
    if (edit) {
      switch (selection) {
        case 0:
          setTime(hour(), minute(), second(), map(analogRead(analogInput), 0, ANALOGLIMIT, 1, 31), month(), year());
          break;
        case 1:
          setTime(hour(), minute(), second(), day(), map(analogRead(analogInput), 0, ANALOGLIMIT, 1, 12), year());
          break;
        case 2:
          setTime(hour(), minute(), second(), day(), month(), map(analogRead(analogInput), 0, ANALOGLIMIT, 2015, 2080));
          break;
        default:
          break;
      }
    }
    if (millis() - lastAction > TIMEOUT) {
      done = true;
    }
    if (readButton()) {
      selection ++;
      pastAnalog = analogRead(analogInput);
      edit = false;
      lastAction = millis();
      if (selection == 3) {
        done = true;
      }
    }
    int irValue = readIR();
    switch (irValue) {
      case RIGHT:
        selection++;
        pastAnalog = analogRead(analogInput);
        edit = false;
        lastAction = millis();
        if (selection == 3) selection = 2;
        break;
      case LEFT:
        selection--;
        pastAnalog = analogRead(analogInput);
        edit = false;
        lastAction = millis();
        if (selection == -1) selection = 0;
        break;
      case ENTER:
        pastAnalog = analogRead(analogInput);
        edit = false;
        lastAction = millis();
        done = true;
        break;
      case UP:
        lastAction = millis();
        switch (selection) {
          case 0:
            setTime(hour(), minute(), second(), day() + 1, month(), year());
            break;
          case 1:
            setTime(hour(), minute(), second(), day(), month() + 1, year());
            break;
          case 2:
            setTime(hour(), minute(), second(), day(), month(), year() + 1);
            break;
          default:
            break;
        }
        break;
      case DOWN:
        lastAction = millis();
        switch (selection) {
          case 0:
            if (day() != 1) setTime(hour(), minute(), second(), day() - 1, month(), year());
            break;
          case 1:
            if (month() == 1) setTime(hour(), minute(), second(), day(), 12, year());
            else setTime(hour(), minute(), second(), day(), month() - 1, year());
            break;
          case 2:
            setTime(hour(), minute(), second(), day(), month(), year() - 1);
            break;
          default:
            break;
        }
        break;
      default:
        break;
    }
    if (done) {
      for (int i = 0; i < 6; i++) {
        blink[i] = false;
      }
    }
  }
}

int readIR() {
  if (millis() - pastIRReading > IRDELAY && !irClean) {
    if (irrecv.decode(&results)) {
      irrecv.resume();
    }
    irClean = true;
  }
  if (irClean) {
    if (irrecv.decode(&results)) {
      irrecv.resume();
      irClean = false;
      pastIRReading = millis();
      String value = String(results.value, HEX);
      //if (/*value.startsWith(String(REMOTECODE)) && */value.length() == 8) {
      value.remove(0, 4);
      //if (value.startsWith("a")) irRemoteState = false;
      //else if (value.startsWith("2")) irRemoteState = true;
      value.remove(0, 1);
      //if (irRemoteState != lastIrRemoteState) {
      //lastIrRemoteState  = irRemoteState;
      if (value == left[0] || value == left[1] || value == left[2]) return LEFT;
      else if (value == right[0] || value == right[1] || value == right[2]) return RIGHT;
      else if (value == up[0] || value == up[1] || value == up[2]) return UP;
      else if (value == down[0] || value == down[1] || value == down[2]) return DOWN;
      else if (value == enter[0] || value == enter[1] || value == enter[2]) return ENTER;
      else if (value == settings[0] || value == settings[1] || value == settings[2]) return SETTINGS;
      else if (value == list[0] || value == list[1] || value == list[2]) return LIST;
      else if (value == zero[0] || value == zero[1] || value == zero[2]) return 0;
      else if (value == one[0] || value == one[1] || value == one[2]) return 1;
      else if (value == two[0] || value == two[1] || value == two[2]) return 2;
      else if (value == three[0] || value == three[1] || value == three[2]) return 3;
      else if (value == four[0] || value == four[1] || value == four[2]) return 4;
      else if (value == five[0] || value == five[1] || value == five[2]) return 5;
      else if (value == six[0] || value == six[1] || value == six[2]) return 6;
      else if (value == seven[0] || value == seven[1] || value == seven[2]) return 7;
      else if (value == eight[0] || value == eight[1] || value == eight[2]) return 8;
      else if (value == nine[0] || value == nine[1] || value == nine[2]) return 9;
      //}
      //}
    }
  }
  return 999;
}

int readButton() {
  if (digitalRead(button) == LOW) {
    unsigned long lastLow = millis();
    unsigned long startTime = millis();
    boolean done = false;
    while (!done) {
      flip();
      if (digitalRead(button) == LOW) {
        lastLow = millis();
      }
      else if (millis() - lastLow > DEBOUNCEDELAY) {
        done = true;
      }
    }
    return ((unsigned long) millis() - startTime);
  }
  else {
    return 0;
  }
}

void alarm() {
  digitalWrite(led, LOW);
  boolean done = false;
  buttonDuration = 0;
  while (!done) {
    for (int i = 0; i < STARWARSLENGTH; i++) {
      if (!done) {
        if (starWarsSong[i][0] != 0) tone(speaker, starWarsSong[i][0]);
        unsigned long pastSongMillis = millis();
        while (millis() - pastSongMillis < starWarsSong[i][2]) {
          flip();
          displayTime(displaySeconds);
          if (readButton()) done = true;
          irMainValue = readIR();
          switch (irMainValue) {
            case ENTER:
              done = true;
              break;
            default:
              break;
          }
        }
        noTone(speaker);
        pastSongMillis = millis();
        while (millis() - pastSongMillis < 10) {
          flip();
          displayTime(displaySeconds);
          if (readButton()) done = true;
          irMainValue = readIR();
          switch (irMainValue) {
            case ENTER:
              done = true;
              break;
            default:
              break;
          }
        }
      }
    }
  }
}

