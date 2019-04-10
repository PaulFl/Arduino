//
//  random_eeprom.h
//  
//
//  Created by Paul Fleury on 10/04/2019.
//

#ifndef random_eeprom_h
#define random_eeprom_h

void BetterRandomSeed(void) {
    unsigned long __seed;
    EEPROM_readAnything(EEPROM_ADDR_SEED, __seed);
    srandom(__seed);
}


long BetterRandom(void) {
    long __seed;
    __seed = random();
    EEPROM_writeAnything(EEPROM_ADDR_SEED, __seed);
    return (__seed);
}


long BetterRandom(long howbig) {
    if (howbig == 0) {
        return 0;
    }
    return (BetterRandom() % howbig);
}


long BetterRandom(long howsmall, long howbig) {
    if (howsmall >= howbig) {
        return howsmall;
    }
    long diff = howbig - howsmall;
    
    return (BetterRandom(diff) + howsmall);
}

#endif /* random_eeprom_h */
