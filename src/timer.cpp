#include "timer.h"
#include "emu.h"

class cpu *cpu;

void timer_init(){
    cpu = get_cpu();
}

void timer::tick() {
    div_counter++;

    if(div_counter == 256){
        Div++;
        div_counter = 0;

    }

    if(tac & 0b100){
        tima_counter++;

        if (tima_counter >= cur_clock_rate()){
            tima_counter = 0;
            tima++;

            if(tima == 0){
                tima = tma;
                cpu->get_interrupt(4);
            }

        }
    }

}

uint8_t timer::read(uint16_t address) {
    switch(address) {
        case 0xFF04:
            return Div;
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

void timer::write(uint16_t addr, uint8_t val) {
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


uint32_t timer::cur_clock_rate() const{
    switch(tac & 0b11){
        case 0: return 1024;
        case 1: return 16;
        case 2: return 64;
        case 3: return 256;
    }
}
