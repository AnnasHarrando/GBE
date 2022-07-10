#include <cstdio>
#include "SquareChannel.h"

uint8_t SquareChannel::read(uint8_t addr) {
    switch (addr) {
        case 0: {
            if (HasSweep) return NR0;
            else return 0;
        }
        case 1: return NR1;
        case 2: return NR2;
        case 3: return NR3;
        case 4: return NR4;
    }
}

void SquareChannel::write(uint8_t addr, uint8_t val) {
    switch (addr) {
        case 0: {
            NR0 = val;
            sweep_timer = ((NR0 >> 4) & 0b111);
        } break;
        case 1: {
            NR0 = val;
            length = 64 - (val & 0b111111);
            cur_wave_form = (val >> 6) & 0b11;
        } break;
        case 2: {
            NR2 = val;
            volume = (NR2 >> 4) & 0b1111;
            envelope_timer = NR2 & 0b111;
            dac_enabled = (val & 0xF8) != 0;
        } break;
        case 3: {
            freq = (freq & 0x700) | val;
            NR3 = val;
        } break;
        case 4: {
            NR4 = val;
            freq = ((val & 0b111) << 8) | (freq & 0xFF);
            if((val >> 7) & 1) trigger();
        } break;
    }
}

void SquareChannel::tick() {
    if(timer > 0) timer--;

    if (timer == 0) {
        wave_duty_position++;
        if (wave_duty_position == 8) wave_duty_position = 0;

        timer = (2048 - freq) * 4;
    }

    if (channel_enabled && dac_enabled) {

        if (wave_form[cur_wave_form][wave_duty_position]) cur_val = volume;
        else cur_val = 0;

    } else cur_val = 0;

}

void SquareChannel::trigger() {

    channel_enabled = true;

    if(length == 0) length = 64;

    timer = (2048 - freq) * 4;
    volume = (NR2 >> 4) & 0b1111;

    envelope_timer = NR2 & 0b111;\
    env_enabled = true;

    shadow_freq = freq;
    sweep_timer = ((NR0 >> 4) & 0b111);
    if(sweep_timer == 0) sweep_timer = 8;
    sweep_enabled = sweep_timer > 0 || (NR0 & 0b111) > 0;
    if((NR0 & 0b111) > 0) calcSweep();
}

bool SquareChannel::len_enabled() {
    return ((NR4 >> 6) & 1);
}

void SquareChannel::fs_tick(uint8_t fs_clock) {
    //printf("%i %i\n",cur_wave_form,wave_duty_position);

    if(fs_clock % 2 == 0){
        if(len_enabled() && length > 0) {
            length--;
            if(length == 0) channel_enabled = false;
        }

    }

    if(fs_clock == 2 || fs_clock == 6){
        if(HasSweep){

            if(sweep_timer > 0) sweep_timer--;

            if(sweep_timer == 0){
                sweep_timer = ((NR0 >> 4) & 0b111);
                if(sweep_timer == 0) sweep_timer = 8;

                if((sweep_enabled && ((NR0 >> 4) & 0b111)) > 0){

                    uint16_t new_freq = calcSweep();

                    if(new_freq < 2048 && (NR0 & 0b111) > 0){

                        freq = new_freq;
                        shadow_freq = new_freq;
                        calcSweep();
                    }
                    calcSweep();
                }
            }

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

uint16_t SquareChannel::calcSweep() {
    uint16_t new_freq = shadow_freq >> (NR0 & 0b111);

    if((NR0 >> 3) & 1) new_freq = shadow_freq - new_freq;
    else new_freq = shadow_freq + new_freq;

    if(new_freq > 2047){
        channel_enabled = false;
    }

    return new_freq;
}


