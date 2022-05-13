#ifndef GBE_PPU_FIFO_H
#define GBE_PPU_FIFO_H

#include "ppu.h"
#include "emu.h"
#include <queue>

typedef enum {
    F_TILE,
    F_DATA0,
    F_DATA1,
    F_SLEEP,
    F_PUSH
}STATE;

typedef struct fifo{
    std::queue<uint32_t> fifo_instance;
    STATE cur_state = F_TILE;
    uint8_t line_x = 0;
    uint8_t pushed_x = 0;
    uint8_t fetch_x = 0;
    uint8_t bgw_fetch_data[3] = {0};
    uint8_t fetch_entry_data[6] = {0};
    uint8_t fetch_count = 0;
    uint8_t map_y = 0;
    uint8_t map_x = 0;
    uint8_t tile_y = 0;
    uint8_t fifo_x = 0;
}fifo;

void init(ppu ppu);
void fifo_load_sprites(oam sprites[40]);
void fifo_set_draw();
bool drawing_stopped();
void fifo_proc();
void fifo_reset();

#endif //GBE_PPU_FIFO_H
