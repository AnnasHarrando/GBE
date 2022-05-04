#ifndef PPU_H
#define PPU_H

#include <cstdint>
#include "lcd.h"

typedef struct{
    uint8_t y;
    uint8_t x;
    uint8_t tile;

    uint8_t c_pm : 3;
    uint8_t c_vram : 1;
    uint8_t pn : 1;
    uint8_t x_flip: 1;
    uint8_t y_flip : 1;
    uint8_t bgp : 1;
}oam;

class ppu {
public:
    lcd *LCD = get_lcd();

    oam oam_ram[40];
    uint8_t vram[0x2000];

    bool dma_active = false;
    uint8_t dma_byte;
    uint8_t dma_val;
    uint8_t dma_delay;

    uint32_t cur_frame = 0;
    uint32_t dots = 0;
    uint32_t *buffer;

    void tick();

    uint8_t oam_read(uint16_t addr);
    void oam_write(uint16_t addr, uint8_t val);

    uint8_t vram_read(uint16_t addr);
    void vram_write(uint16_t addr, uint8_t val);

    void dma_start(uint8_t val);
    void dma_tick();

    void oam_mode();

    void draw_mode();

    void vblank_mode();

    void hblank_mode();

    void inc_ly();
};

#endif