short pwm_outputs[6] = {3,5,5,5,5,5};

//Strobe lights
short outputs[2] = {A1, A2};

int sequence[] = {500, 600, 700, 800};
int sequence_states[] = {true, false, true, false};
int sequence_len = 4;
int sequence_duration;

unsigned long sequence_start_time;
//--


void setup() {
  sequence_duration = sequence[sequence_len - 1];
  for(int i=0; i<6; i++){
    pinMode(pwm_outputs[i], OUTPUT);
  }
  for(int i=0; i<2; i++){
    pinMode(pwm_outputs[i], OUTPUT);
  }

  sequence_start_time = millis();
}

void loop() {
  if (millis() - sequence_start_time > sequence_duration) {
    sequence_start_time = millis();
  }

  bool state = false;
  for (int i=0; i<sequence_len; i++){
    if (millis() - sequence_start_time > sequence[i]){
      state = sequence_states[i];
    }
  }


  for (int i=0; i<2; i++){
    digitalWrite(outputs[i], state);
  }
}
