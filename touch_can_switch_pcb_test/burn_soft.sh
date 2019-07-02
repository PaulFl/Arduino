sudo avrdude -p m328p -c linuxspi -v -P /dev/spidev0.0 -U flash:w:touch_can_switch_pcb_test.arduino.avr.nano.hex
