#ifndef GBE_NOISECHANNEL_H
#define GBE_NOISECHANNEL_H

#include <cstdint>


class NoiseChannel {

public:
    uint8_t divisors[8] = { 8, 16, 32, 48, 64, 80, 96, 112 };
    uint16_t timer = 0;

    uint8_t NR1 = 0;
    uint8_t NR2 = 0;
    uint8_t NR3 = 0;
    uint8_t NR4 = 0;

    uint8_t cur_val = 0;
    uint8_t length = 0;
    uint8_t volume = 0;
    uint8_t envelope_timer = 0;
    uint8_t div_ratio = 0;
    uint8_t clock_shift = 0;

    uint16_t lfsr = 0;

    bool channel_enabled = false;
    bool dac_enabled = false;
    bool env_enabled = false;

    uint8_t read(uint8_t addr) const;
    void write(uint8_t addr, uint8_t val);

    void tick();
    void fs_tick(uint8_t fs_clock);
    void trigger();
    bool len_enabled();
};


#endif //GBE_NOISECHANNEL_H
