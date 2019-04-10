//
//  EEPROM_rw.h
//  
//
//  Created by Paul Fleury on 10/04/2019.
//

#ifndef EEPROM_rw_h
#define EEPROM_rw_h

#include <EEPROM.h>

template <class T> int EEPROM_writeAnything(int ee, const T& value) {
    const byte* p = (const byte*)(const void*)&value;
    int i;
    for (i = 0; i < sizeof(value); i++)
        EEPROM.write(ee++, *p++);
    return i;
}


template <class T> int EEPROM_readAnything(int ee, T& value) {
    byte* p = (byte*)(void*)&value;
    int i;
    for (i = 0; i < sizeof(value); i++)
        *p++ = EEPROM.read(ee++);
    return i;
}

#endif /* EEPROM_rw_h */
