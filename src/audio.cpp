#include <cstdio>
#include "audio.h"



uint8_t audio::read(uint16_t addr) {
    switch(addr){
        case 0xFF10: return channel1.read(0);
        case 0xFF11: return channel1.read(1);
        case 0xFF12: return channel1.read(2);
        case 0xFF13: return channel1.read(3);
        case 0xFF14: return channel1.read(4);
        case 0xFF15:
        case 0xFF1F: return 0;
        case 0xFF16: return channel2.read(1);
        case 0xFF17: return channel2.read(2);
        case 0xFF18: return channel2.read(3);
        case 0xFF19: return channel2.read(4);
        case 0xFF1A: return NR30;
        case 0xFF1B: return NR31;
        case 0xFF1C: return NR32;
        case 0xFF1D: return NR33;
        case 0xFF1E: return NR34;
        case 0xFF20: return channel4.read(1);
        case 0xFF21: return channel4.read(2);
        case 0xFF22: return channel4.read(3);
        case 0xFF23: return channel4.read(4);
        case 0xFF24: return NR50;
        case 0xFF25: return NR51;
        case 0xFF26: return NR52;
        default: return waveform[addr-0xFF30];


    }
}

void audio::write(uint16_t addr, uint8_t val) {
    switch(addr) {
        case 0xFF10:
            channel1.write(0,val); break;
        case 0xFF11:
            channel1.write(1,val); break;
        case 0xFF12:
            channel1.write(2,val); break;
        case 0xFF13:
            channel1.write(3,val); break;
        case 0xFF14:
            channel1.write(4,val); break;
        case 0xFF16:
            channel2.write(1,val); break;
        case 0xFF17:
            channel2.write(2,val); break;
        case 0xFF18:
            channel2.write(3,val); break;
        case 0xFF19:
            channel2.write(4,val); break;
        case 0xFF1A:
            NR30 = val; break;
        case 0xFF1B:
            NR31 = val;
            NR3_length = 256 - val; break;
        case 0xFF1C:
            NR32 = val; break;
        case 0xFF1D:
            NR33 = val; break;
        case 0xFF1E: {
            NR34 = val;
            if((val >> 7) & 1) wave_trigger();
        } break;
        case 0xFF20:
            channel4.write(1,val); break;
        case 0xFF21:
            channel4.write(2,val); break;
        case 0xFF22:
            channel4.write(3,val); break;
        case 0xFF23:
            channel4.write(4,val); break;
        case 0xFF24:
            NR50 = val; break;
        case 0xFF25:
            NR51 = val; break;
        case 0xFF26:
            NR52 = val; break;
    }

    if(addr >= 0xFF30){
        waveform[addr & 0xF] = val;
    }
}

void audio::tick() {

    channel1.tick();
    channel2.tick();
    channel4.tick();

    if(NR3_timer > 0) NR3_timer--;
    if(NR3_timer == 0){

        NR3_timer = (2048 - (uint16_t)(((NR34 & 0b111) << 8) | NR33)) * 2;
        NR3_index++;
        if(NR3_index == 16) NR3_index = 0;

        if(NR3_enabled && ((NR30 >> 7) & 1)){
            if(NR3_index % 2 == 0) NR3_cur_val = ((waveform[NR3_index/2] >> 4) & 0b1111);
            else NR3_cur_val = waveform[NR3_index/2] & 0b1111;

            nr3_get_vol();
        }
        else NR3_cur_val = 0;
    }

}


void audio::nr3_get_vol() {
    switch ((NR32 >> 5) & 0b11) {
        case 0b00: NR3_cur_val = 0; break;
        case 0b10: NR3_cur_val >>= 1; break;
        case 0b11: NR3_cur_val >>= 2; break;
    }
}

void audio::fs_tick() {

    channel1.fs_tick(fs_clock);
    channel2.fs_tick(fs_clock);
    channel4.fs_tick(fs_clock);

    if(fs_clock % 2 == 0){
        if(NR3_length > 0 && NR3_len_enabled){
            NR3_length--;
            if(NR3_length == 0) NR3_enabled = false;
        }
    }

    fs_clock++;
    if(fs_clock == 8) fs_clock = 0;
}

void audio::wave_trigger() {
    NR3_enabled = true;

    if(NR3_length == 0) NR3_length = 256;
    NR3_timer = (2048 - (((NR34 & 0b111) << 8) | NR33))*2;
    NR3_index = 0;
}


