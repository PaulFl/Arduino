short pwm_outputs[6] = {9, 10, 11, 5, 5, 5};
short inputPin = 3;
short switch_position = 1;
bool wingtips_state = true;

//Strobe lights
short outputs[2] = {13, A2};

int sequence[] = {800, 950, 1100, 1250};
int sequence_states[] = {true, false, true, false};
int sequence_len = 4;
int sequence_duration;

unsigned long sequence_start_time;
//--

int led_0 = 0;
int led_1 = 0;
int led_2 = 0;

void setup() {
  Serial.begin(9600);
  sequence_duration = sequence[sequence_len - 1];
  for (int i = 0; i < 6; i++) {
    pinMode(pwm_outputs[i], OUTPUT);
  }
  for (int i = 0; i < 2; i++) {
    pinMode(outputs[i], OUTPUT);
  }

  digitalWrite(pwm_outputs[0], HIGH);
  delay(300);
  digitalWrite(pwm_outputs[0], LOW);
  digitalWrite(pwm_outputs[1], HIGH);
  delay(300);
  digitalWrite(pwm_outputs[1], LOW);
  digitalWrite(pwm_outputs[2], HIGH);
  delay(300);
  digitalWrite(pwm_outputs[2], LOW);
  digitalWrite(pwm_outputs[0], HIGH);
  digitalWrite(pwm_outputs[1], HIGH);
  digitalWrite(pwm_outputs[2], HIGH);
  delay(700);
  digitalWrite(pwm_outputs[0], LOW);
  digitalWrite(pwm_outputs[1], LOW);
  digitalWrite(pwm_outputs[2], LOW);

  sequence_start_time = millis();
}

void loop() {
  int lecture = pulseIn(inputPin, HIGH);
  if (lecture < 1100){
    switch_position = 1;
  } else if (lecture < 1200) {
    switch_position = 2;
  } else if (lecture < 1300) {
    switch_position = 4;
  } else if (lecture < 1700) {
    switch_position = 3;
  } else if (lecture < 1900) {
    switch_position = 5;
  } else {
    switch_position = 6;
  }
  wingtips_state = (switch_position >= 2);
  if (millis() - sequence_start_time > sequence_duration) {
    sequence_start_time = millis();
  }

  bool state = false;
  for (int i = 0; i < sequence_len; i++) {
    if (millis() - sequence_start_time > sequence[i]) {
      state = sequence_states[i];
    }
  }


  for (int i = 0; i < 2; i++) {
    digitalWrite(outputs[i], state && wingtips_state);
  }

    switch (switch_position) {
        case 3:
            led_0 = 255;
            led_1 = 0;
            led_2 = 255;
            break;
        case 4:
            led_0 = 0;
            led_1 = 0;
            led_2 = 255;
            break;
        case 5:
            led_0 = 0;
            led_1 = 255;
            led_2 = 0;
            break;
        case 6:
            led_0 = 255;
            led_1 = 255;
            led_2 = 255;
            break;
            
        default:
            led_0 = 0;
            led_1 = 0;
            led_2 = 0;
            break;
    }
  

  analogWrite(pwm_outputs[0], led_0);
  analogWrite(pwm_outputs[1], led_1);
  analogWrite(pwm_outputs[2], led_2);
}
