#include <SoftPWM.h>
#include <SoftPWM_timer.h>

const short analogOut = 13;
const short analogIn = A1;

void setup() {
  SoftPWMBegin(SOFTPWM_NORMAL);
}

void loop() {
  SoftPWMSet(analogOut, map(analogRead(analogIn), 0, 1023, 255, 0));
}
