#ifndef GBE_IO_H
#define GBE_IO_H

#include <cstdint>
#include "timer.h"
#include "lcd.h"

class io {
public:
    uint8_t serial_data[2] = {0};

    uint8_t read(uint16_t addr);
    void write(uint16_t addr, uint8_t val);

};

#endif