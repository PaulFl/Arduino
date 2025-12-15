const uint8_t PPM_PIN = 1;           // Seeeduino XIAO D2 (change if needed)
const uint8_t SWITCH_PIN = D5;
const uint8_t CHANNEL_COUNT = 6;

const unsigned long FRAME_LEN_US = 20000UL; // total frame length in microseconds (22.5ms)
const unsigned int PULSE_LEN_US = 400;      // short low pulse for each channel (in µs)

unsigned int channels[CHANNEL_COUNT] = {
  1500, // channel 1 (center)
  1500, // channel 2
  1500, // channel 3
  1500, // channel 4
  1500, // channel 5
  1500  // channel 6
};

int mainSheet_pot = 0;
int fine_pot = 0;
int jibSheet_pot = 0;
int switch_state = 0;

int mainSheet_out = 0;
int fine_out = 0;
int jibSheet_out = 0;

void setup() {
  pinMode(PPM_PIN, OUTPUT);
  digitalWrite(PPM_PIN, HIGH);
  pinMode(SWITCH_PIN, INPUT_PULLUP);
}

void loop() {
    jibSheet_pot = analogRead(A2);
    fine_pot = analogRead(A3);
    mainSheet_pot = analogRead(A4);
    switch_state = digitalRead(SWITCH_PIN);

    jibSheet_out = map(jibSheet_pot, 0, 1023, 1000, 2000);
    // fine_out = map(fine_pot, 0, 1023, 0, 500);
    mainSheet_out = map(mainSheet_pot, 0, 1023, 1000, 2000);
    jibSheet_out = constrain(jibSheet_out, 800, 2200);
    // fine_out = constrain(fine_out, 800, 2200);
    mainSheet_out = constrain(mainSheet_out, 800, 2200);

    channels[3] = mainSheet_out;

    if (!switch_state) {
      fine_out = map(fine_pot, 0, 1023, 0, 500);
      channels[4] = 1500 + fine_out;
    } else {
      fine_out = map(fine_pot, 0, 1023, 0, 300);
      channels[4] = 1300 - fine_out;
    }

    // channels[4] = fine_out;
    channels[5] = jibSheet_out;




  unsigned long frameStart = micros();

  // Generate each channel's low pulse + high gap
  for (uint8_t ch = 0; ch < CHANNEL_COUNT; ++ch) {
    unsigned int slot = channels[ch];

    // Bound slot to reasonable RC range
    if (slot < 800) slot = 800;
    if (slot > 2200) slot = 2200;

    // Drive LOW for short pulse (synchronizing pulse)
    noInterrupts();
    digitalWrite(PPM_PIN, LOW);
    delayMicroseconds(PULSE_LEN_US);
    digitalWrite(PPM_PIN, HIGH);
    interrupts();

    // Now keep HIGH for the rest of the slot (slot includes the PULSE_LEN_US)
    unsigned int rest = (slot > PULSE_LEN_US) ? (slot - PULSE_LEN_US) : 0;
    delayMicroseconds(rest);
  }

  // compute remaining time to reach FRAME_LEN_US and wait (this is the sync gap)
  unsigned long elapsed = micros() - frameStart;
  if (elapsed < FRAME_LEN_US) {
    unsigned long remain = FRAME_LEN_US - elapsed;
    delayMicroseconds(remain);
  } else {
    // frame overrun — happens if CHANNEL values/frame too long; no delay, continue immediately
    // Consider adjusting FRAME_LEN_US or reducing channel ranges
  }
}
