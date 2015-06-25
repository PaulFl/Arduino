#include <IRremote.h>


IRrecv irrecv(11); // Receive on pin 11
decode_results results;

void setup() {
	Serial.begin(9600);
	irrecv.enableIRIn(); // Start the receiver
}

void loop() {
	if (irrecv.decode(&results)) {
	Serial.println(results.value, HEX);
	irrecv.resume(); // Continue receiving
	}
}
