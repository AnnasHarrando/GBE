#ifndef GBE_BUS_H
#define GBE_BUS_H

#include "emu.h"

class bus {
public:
    uint8_t read(uint16_t addr);
    void write(uint16_t addr, uint8_t val);

};


#endif //GBE_BUS_H
