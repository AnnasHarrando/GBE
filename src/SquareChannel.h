#ifndef GBE_SQUARECHANNEL_H
#define GBE_SQUARECHANNEL_H


#include <cstdint>

class SquareChannel {
public:
    uint8_t wave_form[4][8] = {
            {0,0,0,0,0,0,0,1},
            {0,0,0,0,0,0,1,1},
            {0,0,0,0,1,1,1,1},
            {1,1,1,1,1,1,0,0}
    };

    bool channel_enabled = false;
    bool dac_enabled = false;
    bool env_enabled = false;

    uint8_t NR0 = 0;
    uint8_t NR1 = 0;
    uint8_t NR2 = 0;
    uint8_t NR3 = 0;
    uint8_t NR4 = 0;

    uint32_t timer = 0;
    uint8_t wave_duty_position = 0;
    uint32_t length = 0;
    uint8_t cur_val = 0;
    uint8_t cur_wave_form = 0;
    uint8_t volume = 0;
    uint16_t freq = 0;

    uint16_t shadow_freq = 0;
    uint8_t sweep_timer = 0;
    bool sweep_enabled = false;
    uint16_t calcSweep();

    uint8_t envelope_timer = 0;

    bool HasSweep = false;

    SquareChannel(bool val){
        HasSweep = val;
    };

    uint8_t read(uint8_t addr);
    void write(uint8_t addr, uint8_t val);

    void tick();
    void fs_tick(uint8_t fs_clock);
    void trigger();
    bool len_enabled();

};


#endif //GBE_SQUARECHANNEL_H
