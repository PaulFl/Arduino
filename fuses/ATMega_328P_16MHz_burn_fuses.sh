sudo avrdude -p m328p -c linuxspi -v -P /dev/spidev0.0 -U lock:w:lock.hex:i -U efuse:w:efuse.hex:i -U hfuse:w:hfuse.hex:i -U lfuse:w:lfuse.hex:i -b 19200
