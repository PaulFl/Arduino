#include <IRremote.h>


IRrecv irrecv(7);
decode_results results;
String value;

void setup() {
	Serial.begin(9600);
	irrecv.enableIRIn();
  irrecv.blink13(true);
}

void loop() {
	if (irrecv.decode(&results)) {
  value = String(results.value, HEX);
 Serial.println(value);
	irrecv.resume();
	}
}
