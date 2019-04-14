sudo avrdude -p m328p -c linuxspi -v -P /dev/spidev0.0 -U flash:w:simon_game_2.5.hex
