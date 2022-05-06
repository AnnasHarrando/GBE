//
// Created by annas on 05/05/2022.
//

#include "ppu_fifo.h"

static ppu PPU;
static fifo FIFO;

void init(ppu ppu){
    PPU = ppu;
}

void fifo_set_draw(){
    FIFO.cur_state = F_TILE;
    FIFO.line_x = 0;
    FIFO.fetch_x = 0;
    FIFO.pushed_x = 0;
    FIFO.fifo_x = 0;
}

bool drawing_stopped(){
    return FIFO.pushed_x >= 160;
}

uint32_t fifo_pop(){
    uint32_t temp = FIFO.fifo_instance.front();
    FIFO.fifo_instance.pop();
    return temp;
}

void fifo_push(uint32_t val){
    FIFO.fifo_instance.push(val);
}

void fifo_draw_pixel(){
    if(FIFO.fifo_instance.size() > 8){
        uint32_t temp = fifo_pop();

        if(FIFO.line_x >= (PPU.LCD->x_scroll % 8)){
            //printf("%i\n",FIFO.pushed_x + PPU.LCD->ly * 160);
            PPU.buffer[FIFO.pushed_x + (PPU.LCD->ly * 160)] = temp;
            FIFO.pushed_x++;
        }

        FIFO.line_x++;
    }
}

bool fifo_push_pixel(){
    if(FIFO.fifo_instance.size() > 8) return false;

    int x = FIFO.fetch_x - (8-(PPU.LCD->x_scroll % 8));

    for (int i=0; i<8; i++) {
        int bit = 7 - i;

        uint8_t low = !!(FIFO.bgw_fetch_data[1] & (1 << bit));
        uint8_t high = !!(FIFO.bgw_fetch_data[2] & (1 << bit)) << 1;

        uint32_t color = PPU.LCD->bg_color[high | low];
        if(x >= 0) {
            fifo_push(color);
            FIFO.fifo_x++;
        }
    }

    return true;
}

void fifo_fetch(){
    switch(FIFO.cur_state){
        case F_TILE:
            if(BGW_enabled()){
                FIFO.bgw_fetch_data[0] = bus_read(BG_map_loc() + (FIFO.map_x / 8) + ((FIFO.map_y / 8) * 32));

                if(BGW_data_loc() == 0x8800) FIFO.bgw_fetch_data[0] += 128;
            }
            FIFO.cur_state = F_DATA0;
            FIFO.fetch_x += 8;
            break;
        case F_DATA0:
            FIFO.bgw_fetch_data[1] = bus_read(BGW_data_loc() + (FIFO.bgw_fetch_data[0] * 16) + FIFO.tile_y);
            FIFO.cur_state = F_DATA1;
            break;
        case F_DATA1:
            FIFO.bgw_fetch_data[2] = bus_read(BGW_data_loc() + (FIFO.bgw_fetch_data[0] * 16) + FIFO.tile_y + 1);
            FIFO.cur_state = F_SLEEP;
            break;
        case F_SLEEP:
            FIFO.cur_state = F_PUSH;
            break;
        case F_PUSH:
            if(fifo_push_pixel()) FIFO.cur_state = F_TILE;
            break;
    }
}

void fifo_proc(){

    FIFO.map_x = FIFO.fetch_x + PPU.LCD->x_scroll;
    FIFO.map_y = PPU.LCD->ly + PPU.LCD->y_scroll;
    FIFO.tile_y = ((PPU.LCD->ly + PPU.LCD->y_scroll) % 8) *2;

    if(!(PPU.dots & 1)) fifo_fetch();

    fifo_draw_pixel();

}
void fifo_reset(){
    while(!FIFO.fifo_instance.empty()) fifo_pop();
}