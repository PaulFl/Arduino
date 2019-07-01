#include <CapacitiveSensor.h>

#define DEBUG

//User parameters

int highest_high_threshold = 15000;
int lowest_high_threshold = 1500;

int cap_sensor_samples = 100;

int threshold_hysteresis = 300;

int led_pause = 5000;
int speed = 100;
int dim_color = 10;

//------

//Default values
int threshold_high_value = 6600;
int threshold_low_value = 6300;

//--

//Pins
short status_led_pin = 13;
short red_led_pin = 3;
short green_led_pin = 6;
short blue_led_pin = 9;
short pot_pin = A0;
short can_send_pin = 4;
short can_rcv_pin = 7;
short relay_pin = 2;

int rgb_leds_pin[] = {red_led_pin, green_led_pin, blue_led_pin};
//--

CapacitiveSensor cap_sensor = CapacitiveSensor(can_send_pin, can_rcv_pin);

int pot_value = 0;
bool pot_end_reached = false;

long sensor_value;
bool touched = false;

int new_colors[] = {0,0,0};
int values_colors[] = {0,0,0};
int color;
bool color_reached = false;

unsigned long last_change;
unsigned long last_color;

bool relay_state = LOW;
bool play_color = true;


void setup() {
#ifdef DEBUG
    Serial.begin(9600);
#endif
    
    pinMode(status_led_pin, OUTPUT);
    pinMode(relay_pin, OUTPUT);
    
    digitalWrite(status_led_pin, LOW);
    digitalWrite(relay_pin, LOW);
    
    
    for (int i = 0; i<3; i++) {
        pinMode(rgb_leds_pin[i], OUTPUT);
    }
    
    last_color = millis();
    last_change = millis();
}

void loop() {
    set_threshold();
    read_cap_sensor();
    
    if (sensor_value > threshold_high_value && !touched) {
        touched = true;
        for (int i = 0; i<3; i++) {
            digitalWrite(rgb_leds_pin[i], LOW)
        }
        relay_state = !relay_state;
        digitalWrite(relay_pin, relay_state);
#ifdef DEBUG
        Serial.println("TRIGGERED");
#endif
    } else if (sensor_value < threshold_low_value && touched) {
        touched = false;
        for (int i =0; i<3; i++) {
            analogWrite(rgb_leds_pin[i], values_colors[i])
        }
    }
    
    update_color();
    
#ifdef DEBUG
    values_to_serial();
#endif
}

void read_cap_sensor() {
    sensor_value = cap_sensor.capacitiveSensorRaw(cap_sensor_samples);
}

void set_threshold() {
    pot_value = analogRead(pot_pin);
    if (pot_value > 1021 || pot_value < 2) {
        pot_end_reached = true;
    } else {
        pot_end_reached = false;
    }
    threshold_high_value = map(pot_value, 0, 1023, lowest_high_threshold, highest_high_threshold);
    threshold_low_value = threshold_high_value - threshold_hysteresis;
}

void values_to_serial() {
    Serial.print("Pot: ");
    Serial.print(pot_value);
    Serial.print("\t");
    Serial.print("Threshold: ");
    Serial.print(threshold_high_value);
    Serial.print("\t");
    Serial.print("Sensor: ");
    Serial.print(sensor_value);
    Serial.println();
}

void switch_relay(bool state) {
    digitalWrite(relay_pin, state);
}

void update_color() {
    
}

void set_color() {
    
}
