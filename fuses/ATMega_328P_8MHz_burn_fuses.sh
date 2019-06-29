sudo avrdude -p m328p -c linuxspi -v -P /dev/spidev0.0 -U lock:w:0x3F:m  -U efuse:w:0x05:m -U hfuse:w:0xDA:m -U lfuse:w:0xE2:m -b 19200
