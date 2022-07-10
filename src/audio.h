#ifndef GBE_AUDIO_H
#define GBE_AUDIO_H

#include "SDL.h"
#include "SquareChannel.h"
#include "NoiseChannel.h"

class audio{
public:

    uint8_t waveform[16] = {0};

    uint8_t cur_val = 0;

    SquareChannel channel1 = SquareChannel(true);
    SquareChannel channel2 = SquareChannel(false);
    NoiseChannel channel4 = NoiseChannel();

    uint8_t NR30 = 0;
    uint8_t NR31 = 0;
    uint8_t NR32 = 0;
    uint8_t NR33 = 0;
    uint8_t NR34 = 0;

    uint32_t NR3_timer = 0;
    uint16_t NR3_length = 0;
    uint8_t NR3_index = 0;
    uint8_t NR3_cur_val = 0;
    bool NR3_enabled = false;

    uint8_t NR50 = 0;
    uint8_t NR51 = 0;
    uint8_t NR52 = 0;

    uint32_t index = 0;
    uint8_t fs_clock = 0;

    uint8_t read(uint16_t addr);
    void write(uint16_t addr, uint8_t val);
    void tick();
    void fs_tick();

    uint8_t left_vol(){
        return ((NR50 >> 4) & 0b111);
    }

    uint8_t right_vol(){
        return (NR50 & 0b111);
    };

    void wave_trigger();
    void nr3_get_vol();


    bool sound_enabled() {
        return ((NR52 >> 7) & 1);
    }

#define NR3_len_enabled ((NR34 >> 6) & 1)

};



#endif //GBE_AUDIO_H
