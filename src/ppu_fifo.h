#ifndef GBE_PPU_FIFO_H
#define GBE_PPU_FIFO_H

#include "ppu.h"
#include <queue>

typedef struct {
    uint32_t color;
    bool isWindow;
}pixel;


typedef enum {
    F_TILE,
    F_DATA0,
    F_DATA1,
    F_SLEEP,
    F_PUSH
}STATE;

class ppu_fifo {
public:
    std::queue<pixel> fifo_pipeline;
    STATE cur_state = F_TILE;
    uint8_t line_x = 0;
    uint8_t pushed_x = 0;
    uint8_t fetch_x = 0;
    uint8_t bgw_fetch_data[3] = {0};
    uint8_t fetch_entry_data[6] = {0};
    uint8_t map_y = 0;
    uint8_t map_x = 0;
    uint8_t tile_y = 0;
    uint8_t fifo_x = 0;

    void load_sprites();
    void set_draw();
    bool drawing_stopped();
    void proc();
    void reset();
    bool push_pixel();
    void load_sprite_tile();
    void load_sprite_data(uint8_t val);
    bool window_in_x();
    bool window_in_y();
    void load_window();
    void fetch();
    pixel pop();
    void push(pixel val);
    void draw_pixel();
    uint32_t get_sprite(uint8_t bit, uint32_t color, uint8_t bg);
};

void fifo_init();
#endif //GBE_PPU_FIFO_H
