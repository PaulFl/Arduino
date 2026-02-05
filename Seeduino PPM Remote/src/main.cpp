#include <Arduino.h>

const int CHANNEL_COUNT = 8;
const unsigned long FRAME_LEN_US = 20000UL; // total frame length in microseconds (22.5ms)
const unsigned int PULSE_LEN_US = 400;      // short low pulse for each channel (in µs)

const int ppm_pin = A0;

const int battery_voltage_pin = A10;

const int channel_pins[6] = {
  A1,
  A2,
  A3,
  A4,
  A5,
  A6
};

int analog_values [CHANNEL_COUNT] = {
  0, 0, 0, 0, 0, 0, 0, 0
};

int battery_voltage_analog_value = 0;
int battery_voltage_mv = 0;
volatile int battery_percent_rounded = 0;

int ppm_values [CHANNEL_COUNT] = {
  1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000
};

const int led_pin = A9;


void generate_ppm_frame() {
  unsigned long frame_start = micros();

  // Generate each channel's low pulse + high gap
  // noInterrupts(); // Disable interrupts during critical timing section
  for (uint8_t ch = 0; ch < CHANNEL_COUNT; ++ch) {
    unsigned int slot = ppm_values[ch];

    // Bound slot to reasonable RC range
    if (slot < 800) slot = 800;
    if (slot > 2200) slot = 2200;

    // Drive LOW for short pulse (synchronizing pulse)
    noInterrupts();
    digitalWrite(ppm_pin, LOW);
    delayMicroseconds(PULSE_LEN_US);
    digitalWrite(ppm_pin, HIGH);
    interrupts();
    
    // Now keep HIGH for the rest of the slot (slot includes the PULSE_LEN_US)
    unsigned int rest = (slot > PULSE_LEN_US) ? (slot - PULSE_LEN_US) : 0;
    delayMicroseconds(rest);
  }
  // interrupts();

 // compute remaining time to reach FRAME_LEN_US and wait (this is the sync gap)
  unsigned long elapsed = micros() - frame_start;
  if (elapsed < FRAME_LEN_US) {
    unsigned long remain = FRAME_LEN_US - elapsed;
    delayMicroseconds(remain);
  } else {
    // frame overrun — happens if CHANNEL values/frame too long; no delay, continue immediately
    // Consider adjusting FRAME_LEN_US or reducing channel ranges
  }
}

void led_blink() {
  static int led_state = 0;
  static unsigned long last_millis = 0;

  unsigned long current_millis = millis();

  unsigned long on_time = battery_percent_rounded * 10; 
  unsigned long off_time = 1000 - on_time;

  if (!led_state && (current_millis - last_millis > off_time)) {
    led_state = 1;
    digitalWrite(led_pin, led_state);
    last_millis = current_millis;
  } else if (led_state && (current_millis - last_millis > on_time)) {
    led_state = 0;
    digitalWrite(led_pin, led_state);
    last_millis = current_millis;
  }
}


void setup() {
  pinMode(ppm_pin, OUTPUT);
  pinMode(led_pin, OUTPUT);

  digitalWrite(led_pin, HIGH);

  Serial.begin(115200);

  delay(3000);
}

void loop() {
  for (int i = 0; i < 6; i++) {
    analog_values[i] = analogRead(channel_pins[i]);
    ppm_values[i] = map(analog_values[i], 0, 1023, 1000, 2000);
  }

  battery_voltage_analog_value = analogRead(battery_voltage_pin);
  battery_voltage_mv = map(battery_voltage_analog_value, 0, 1023, 0, 6600);

  int raw_percent = map(battery_voltage_mv, 3300, 4200, 0, 100);
  raw_percent = constrain(raw_percent, 5, 100);

  battery_percent_rounded = ((raw_percent + 5) / 10) * 10;

  Serial.print("Battery: ");
  Serial.print(battery_voltage_analog_value);
  Serial.print(" (raw) / ");
  Serial.print(battery_voltage_mv);
  Serial.print(" mV / ");
  Serial.print(battery_percent_rounded);
  Serial.println("%");

  for (int i = 0; i < 6; i++) {
    Serial.print("CH");
    Serial.print(i + 1);
    Serial.print(": ");
    Serial.print(analog_values[i]);
    Serial.print(" -> ");
    Serial.print(ppm_values[i]);
    if (i < 5) Serial.print("\t");
  }
  Serial.println();

  generate_ppm_frame();
  led_blink();
}