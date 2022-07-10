#ifndef GBE_TIMER_H
#define GBE_TIMER_H

#include <cstdint>

class timer {
public:
    uint16_t div_counter = 0;
    uint32_t tima_counter = 0;

    uint8_t Div = 0;
    uint8_t tima;
    uint8_t tma;
    uint8_t tac;

    void tick();
    uint8_t read(uint16_t address);
    void write(uint16_t addr, uint8_t val);
    uint32_t cur_clock_rate() const;
};

void timer_init();

#endif GBE_TIMER_H
