#include "timer.h"

static uint16_t Div = 0xAC00;
static uint8_t tima;
static uint8_t tma;
static uint8_t tac;

void tick() {
    uint16_t prev_div = Div;

    Div++;

    bool timer_update = false;

    switch(tac & (0b11)) {
        case 0b00:
            timer_update = (prev_div & (1 << 9)) && (!(Div & (1 << 9)));
            break;
        case 0b01:
            timer_update = (prev_div & (1 << 3)) && (!(Div & (1 << 3)));
            break;
        case 0b10:
            timer_update = (prev_div & (1 << 5)) && (!(Div & (1 << 5)));
            break;
        case 0b11:
            timer_update = (prev_div & (1 << 7)) && (!(Div & (1 << 7)));
            break;
    }

    if (timer_update && tac & (1 << 2)) {
        tima++;

        if (tima == 0xFF) {
            tima = tma;

            get_interrupt(4);
        }
    }
}

void timer_write(uint16_t addr, uint8_t val){
    switch(addr) {
        case 0xFF04:
            Div = 0;
            break;
        case 0xFF05:
            tima = val;
            break;
        case 0xFF06:
            tma = val;
            break;
        case 0xFF07:
            tac = val;
            break;
        default:
            break;
    }
}

uint8_t timer_read(uint16_t address) {
    switch(address) {
        case 0xFF04:
            return Div >> 8;
        case 0xFF05:
            return tima;
        case 0xFF06:
            return tma;
        case 0xFF07:
            return tac;
        default:
            break;
    }
}