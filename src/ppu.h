#include <cstdint>

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
    oam oam_ram[40];
    uint8_t vram[0x2000];

    bool dma_active = false;
    uint8_t dma_byte;
    uint8_t dma_val;
    uint8_t dma_delay;

    uint8_t oam_read(uint16_t addr);
    void oam_write(uint16_t addr, uint8_t val);

    uint8_t vram_read(uint16_t addr);
    void vram_write(uint16_t addr, uint8_t val);

    void dma_start(uint8_t val);
    void dma_tick();
};
