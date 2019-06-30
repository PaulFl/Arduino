#include <CapacitiveSensor.h>

#define DEBUG

int cap_sensor_samples = 100;
int threshold_value;

short status_led_pin = 13;
short red_led_pin = 3;
short green_led_pin = 6;
short blue_led_pin = 9;
short pot_pin = A0;
short can_send_pin = 4;
short can_rcv_pin = 7;
short relay_pin = 2;

int rgb_leds_pin[] = {red_led_pin, green_led_pin, blue_led_pin};

CapacitiveSensor cap_sensor = CapacitiveSensor(can_send_pin, can_rcv_pin);

int pot_value = 0;
int sensor_value;


void setup() {
#ifdef DEBUG
    Serial.begin(9600);
#endif
    
    pinMode(status_led_pin, OUTPUT);
    for (int i = 0; i<3; i++) {
        pinMode(rgb_leds_pin[i], OUTPUT);
    }
    
    
}

void loop() {
    set_threshold();
    cap_sensor();
    
    
#ifdef DEBUG
    values_to_serial();
#endif
}

void cap_sensor() {
    sensor_value = cap_sensor.capacitiveSensorRaw(cap_sensor_samples);
}

void set_threshold() {
    pot_value = analogRead(pot_pin);
}

void values_to_serial() {
    Serial.print("Pot: ");
    Serial.print(pot_value);
    Serial.print("\t");
    Serial.print("Threshold: ");
    Serial.print(threshold_value);
    Serial.print("\t");
    Serial.print("Sensor: ");
    Serial.print(sensor_value);
    Serial.println();
}

void switch_relay(bool state) {
    digitalWrite(relay_pin, state);
}
