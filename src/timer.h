#ifndef GBE_TIMER_H
#define GBE_TIMER_H

#include <cstdint>

class timer {
public:
    uint16_t Div = 0;
    uint8_t tima;
    uint8_t tma;
    uint8_t tac;

    void tick();
    uint8_t read(uint16_t address);
    void write(uint16_t addr, uint8_t val);
};

void timer_init();

#endif GBE_TIMER_H
