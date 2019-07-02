/* Touch can pcb test
 Paul Fleury 24/02/2019 */

/* TEST:
 Status LED
 Relay
 RGB LEDs
 Power supply
 Trim Pot
 Serial port
 Can sensor
 */

#include <CapacitiveSensor.h>

int sensor_samples = 100;


short status_led_pin = 13;
short red_led_pin = 3;
short green_led_pin = 6;
short blue_led_pin = 9;
short pot_pin = A0;
short can_send_pin = 4;
short can_rcv_pin = 7;
short relay_pin = 2;

CapacitiveSensor cap_sensor = CapacitiveSensor(can_send_pin, can_rcv_pin);

void setup() {
    Serial.begin(9600);
    
    pinMode(status_led_pin, OUTPUT);
    pinMode(relay_pin, OUTPUT);
    pinMode(red_led_pin, OUTPUT);
    pinMode(green_led_pin, OUTPUT);
    pinMode(blue_led_pin, OUTPUT);
    
    status_led_test();
    relay_test();
    rgb_led_test();
    power_supply_test();
    serial_port_test();
    while (1) {
        pot_test();
        can_sensor_test();
    }
}

void loop() {
    
}

void status_led_test() {
    for (int i = 0; i<10; i++) {
        digitalWrite(status_led_pin, HIGH);
        delay(1000 - 100*i);
        digitalWrite(status_led_pin, LOW);
        delay(1000 - 100*i);
    }
    
    digitalWrite(status_led_pin, HIGH);
}

void relay_test() {
    digitalWrite(relay_pin, HIGH);
    delay(5000);
    digitalWrite(relay_pin, LOW);
    
    for (int i = 0; i < 20; i++) {
        digitalWrite(relay_pin, HIGH);
        delay(500 - 23*i);
        digitalWrite(relay_pin, LOW);
        delay(500);
    }
}

void rgb_led_test() {
    digitalWrite(red_led_pin, HIGH);
    delay(500);
    digitalWrite(red_led_pin, LOW);
    digitalWrite(green_led_pin, HIGH);
    delay(500);
    digitalWrite(green_led_pin, LOW);
    digitalWrite(blue_led_pin, HIGH);
    delay(500);
    digitalWrite(blue_led_pin, LOW);
    delay(500);
    
    digitalWrite(red_led_pin, HIGH);
    digitalWrite(green_led_pin, HIGH);
    digitalWrite(blue_led_pin, HIGH);
    delay(1000);
    
    digitalWrite(red_led_pin, LOW);
    delay(500);
    digitalWrite(red_led_pin, HIGH);
    digitalWrite(green_led_pin, LOW);
    delay(500);
    digitalWrite(green_led_pin, HIGH);
    digitalWrite(blue_led_pin, LOW);
    delay(500);
    
    digitalWrite(red_led_pin, LOW);
    digitalWrite(green_led_pin, LOW);
    
    for (int i = 0; i <=255; i++) {
        analogWrite(red_led_pin, i);
        delay(12);
    }
    digitalWrite(red_led_pin, LOW);
    
    for (int i = 0; i <=255; i++) {
        analogWrite(green_led_pin, i);
        delay(12);
    }
    digitalWrite(green_led_pin, LOW);
    
    for (int i = 0; i <=255; i++) {
        analogWrite(blue_led_pin, i);
        delay(12);
    }
    digitalWrite(blue_led_pin, LOW);
}

void power_supply_test() {
    digitalWrite(relay_pin, HIGH);
    digitalWrite(status_led_test, HIGH);
    digitalWrite(red_led_pin, HIGH);
    digitalWrite(green_led_pin, HIGH);
    digitalWrite(blue_led_pin, HIGH);
    
    delay(10000);
    
    digitalWrite(relay_pin, LOW);
    digitalWrite(status_led_test, LOW);
    digitalWrite(red_led_pin, LOW);
    digitalWrite(green_led_pin, LOW);
    digitalWrite(blue_led_pin, LOW);
}

void pot_test() {
    Serial.print("Pot: \t");
    Serial.print(analogRead(pot_pin));
    Serial.print("\t");
}

void serial_port_test() {
    Serial.println("Serial test");
}

void can_sensor_test() {
    Serial.print("Sensor: \t");
    Serial.print(cap_sensor.capacitiveSensorRaw(sensor_samples));
    Serial.println("\t");
}
