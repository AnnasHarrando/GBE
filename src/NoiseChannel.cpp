
#include "NoiseChannel.h"

uint8_t NoiseChannel::read(uint8_t addr) const {
    switch (addr) {
        case 0: return 0;
        case 1: return NR1;
        case 2: return NR2;
        case 3: return NR3;
        case 4: return NR4;
    }
}

void NoiseChannel::write(uint8_t addr, uint8_t val)  {
    switch (addr) {
        case 0: break;

        case 1: NR1 = val;
        length = 64 - (val & 0x3F); break;

        case 2:NR2 = val;
        volume = (NR2 >> 4) & 0b1111;
        envelope_timer = NR2 & 0b111;
        dac_enabled = (val & 0xF8) != 0;
        break;

        case 3: NR3 = val;
        div_ratio = val & 0b111;
        clock_shift = ((val >> 4) & 0b111);
        break;
        case 4: NR4 = val;
        if((val >> 7) & 1) trigger(); break;
    }
}

bool NoiseChannel::len_enabled() {
    return ((NR4 >> 6) & 1);
}

void NoiseChannel::trigger() {

    channel_enabled = true;
    if(length == 0) length = 64;
    timer = divisors[div_ratio] << clock_shift;
    envelope_timer = NR2 & 0b111;
    env_enabled = true;
    volume = (NR2 >> 4) & 0b1111;
    lfsr = 0x7FFF;
}

void NoiseChannel::tick() {
    if(timer > 0) timer--;

    if (timer == 0) {

        timer = divisors[div_ratio] << clock_shift;
        uint8_t temp = (lfsr & 0x1) ^ ((lfsr >> 1) & 0x1);
        lfsr >>= 1;
        lfsr |= temp << 14;

        if((NR3 & 0x8) == 0x8){
            lfsr &= ~0x40;
            lfsr |= temp << 6;
        }

        if (channel_enabled && dac_enabled && (lfsr & 0x1) == 0) {
            cur_val = volume;
        }
        else {
            cur_val = 0;
        }
    }
}

void NoiseChannel::fs_tick(uint8_t fs_clock) {

    if(fs_clock % 2 == 0){
        if(len_enabled() && length > 0) {
            length--;
            if(length == 0) channel_enabled = false;
        }

    }


    if(fs_clock == 7){

        if(envelope_timer > 0) envelope_timer--;

        if(envelope_timer == 0){
            envelope_timer = NR2 & 0b111;
            if(envelope_timer == 0) envelope_timer = 8;


            if((NR2 & 0b111) > 0 && env_enabled) {
                if ((volume < 0xF && ((NR2 >> 3) & 1)) | (volume > 0 && !((NR2 >> 3) & 1))) {
                    if ((NR2 >> 3) & 1) volume++;
                    else volume--;
                }

                if(volume == 0 || volume == 0xF) env_enabled = false;
            }
        }
    }
}
