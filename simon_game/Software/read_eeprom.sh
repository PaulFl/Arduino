sudo avrdude -p m328p -c linuxspi -v -P /dev/spidev0.0 -U eeprom:r:eeprom_data.hex:i
