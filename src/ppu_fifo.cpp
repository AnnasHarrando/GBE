#include "ppu_fifo.h"
#include <algorithm>

static ppu *PPU;
static fifo FIFO;

void init(ppu *ppu){
    PPU = ppu;
}

bool compare(oam left,oam right){ return left.x < right.x;}

void fifo_load_sprites() {
    memset(PPU->line_sprites,0,10);
    PPU->line_sprites_size = 0;

    uint8_t height = get_sprite_height();

    for (int i = 0; i < 40; i++){
        if(PPU->line_sprites_size == 10) break;

        oam sprite = PPU->oam_ram[i];

        if(sprite.x > 0 && (sprite.y <= PPU->LCD->ly + 16 && sprite.y + height > PPU->LCD->ly + 16)){
            PPU->line_sprites[PPU->line_sprites_size] = sprite;
            PPU->line_sprites_size++;
        }
    }

    std::sort(PPU->line_sprites,PPU->line_sprites+PPU->line_sprites_size, compare);
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

pixel fifo_pop(){
    pixel temp = FIFO.fifo_pipeline.front();
    FIFO.fifo_pipeline.pop();
    return temp;
}

void fifo_push(pixel val){
    FIFO.fifo_pipeline.push(val);
}

void fifo_draw_pixel(){
    if(FIFO.fifo_pipeline.size() > 8){
        pixel temp = fifo_pop();

        if(FIFO.line_x >= (PPU->LCD->x_scroll % 8) || temp.isWindow){
            if(FIFO.window_pixel_size > 0) FIFO.window_pixel_size--;
            PPU->buffer[FIFO.pushed_x + (PPU->LCD->ly * 160)] = temp.color;
            FIFO.pushed_x++;
        }

        FIFO.line_x++;
    }
}

uint32_t get_sprite(uint8_t bit, uint32_t color, uint8_t bg){

    uint32_t temp = color;
    for(int i=0; i < PPU->fetch_entry_size; i++){
        int x = PPU->fetch_entry[i].x - 8 + (PPU->LCD->x_scroll % 8);
        int offset = FIFO.fifo_x - x;

        if(x+8 >= FIFO.fifo_x && (offset >= 0 && offset <= 7)){
            if(PPU->fetch_entry[i].x_flip) bit = offset;
            else bit = (7 - offset);

            uint8_t low = !!(FIFO.fetch_entry_data[i * 2] & (1 << bit));
            uint8_t high = !!(FIFO.fetch_entry_data[(i * 2) + 1] & (1 << bit)) << 1;

            if(high | low){
                if(!PPU->fetch_entry[i].bgp || bg == 0){
                    temp = (PPU->fetch_entry[i].pn) ?
                            PPU->LCD->sp2_color[high | low] : PPU->LCD->sp1_color[high | low];
                }
                break;
            }
        }
    }

    return temp;
}

bool fifo_push_pixel(){
    //if(FIFO.fifo_pipeline.size() > 8) return false;

    //int x = FIFO.fetch_x - (8-(PPU->LCD->x_scroll % 8));

    for (int i=0; i<8; i++) {
        uint32_t color;
        int bit = 7 - i;

        uint8_t low = !!(FIFO.bgw_fetch_data[1] & (1 << bit));
        uint8_t high = !!(FIFO.bgw_fetch_data[2] & (1 << bit)) << 1;

        color = PPU->LCD->bg_color[high | low];

        if(!BGW_enabled()) color = PPU->LCD->bg_color[0];

        if(OBJ_enabled()) color = get_sprite(bit, color, high | low);

        pixel temp = {color,window_enabled()};

        //if(x >= 0) {
            fifo_push(temp);
            FIFO.fifo_x++;
        //}
    }

    return true;
}

void fifo_load_sprite_tile(){
    for(int i = 0; i < PPU->line_sprites_size; i++){
        int x = PPU->line_sprites[i].x - 8 + (PPU->LCD->x_scroll % 8);

        if((x >= FIFO.fetch_x && x < FIFO.fetch_x + 8) || (x+8 >= FIFO.fetch_x && x+8 < FIFO.fetch_x + 8)){
            PPU->fetch_entry[PPU->fetch_entry_size] = PPU->line_sprites[i];
            PPU->fetch_entry_size++;
        }

        if(PPU->fetch_entry_size >=3) break;
    }
}

void fifo_load_sprite_data(uint8_t val){
    uint8_t height = get_sprite_height();

    for(int i=0; i < PPU->fetch_entry_size; i++){
        uint8_t y = (PPU->LCD->ly + 16 - PPU->fetch_entry[i].y) * 2;
        if(PPU->fetch_entry[i].y_flip) y = height*2 - 2 - y;

        uint8_t index = PPU->fetch_entry[i].tile;
        if(height == 16) index &= ~1;

        FIFO.fetch_entry_data[i*2 + val] = bus_read(0x8000 + index*16 + y + val);
    }
}

bool window_in_x(){
    return FIFO.fetch_x + 7 >= PPU->LCD->x_win && FIFO.fetch_x + 7 < PPU->LCD->x_win + 144 + 14;
}

bool window_in_y(){
    return PPU->LCD->ly >= PPU->LCD->y_win && PPU->LCD->ly < PPU->LCD->y_win + 160;
}

void fifo_load_window(){
    if(window_enabled()) {
        if (window_in_x() && window_in_y()) {

            uint8_t y = PPU->window_line / 8;
            FIFO.bgw_fetch_data[0] = bus_read(win_map_loc() + ((FIFO.fetch_x + 7 - PPU->LCD->x_win) / 8) + (y * 32));
            if (BGW_data_loc() == 0x8800) FIFO.bgw_fetch_data[0] += 128;
        }
    }

}

void fifo_fetch(){
    switch(FIFO.cur_state){
        case F_TILE:
            PPU->fetch_entry_size = 0;
            if(BGW_enabled()){
                FIFO.bgw_fetch_data[0] = bus_read(BG_map_loc() + (FIFO.map_x / 8) + ((FIFO.map_y / 8) * 32));

                if(BGW_data_loc() == 0x8800) FIFO.bgw_fetch_data[0] += 128;

                fifo_load_window();
            }

            if(OBJ_enabled() && PPU->line_sprites_size > 0) {
                fifo_load_sprite_tile();
            }
            FIFO.cur_state = F_DATA0;
            FIFO.fetch_x += 8;
            break;
        case F_DATA0:
            FIFO.bgw_fetch_data[1] = bus_read(BGW_data_loc() + (FIFO.bgw_fetch_data[0] * 16) + FIFO.tile_y);

            if(OBJ_enabled())fifo_load_sprite_data(0);

            FIFO.cur_state = F_DATA1;
            break;
        case F_DATA1:
            FIFO.bgw_fetch_data[2] = bus_read(BGW_data_loc() + (FIFO.bgw_fetch_data[0] * 16) + FIFO.tile_y + 1);

            if(OBJ_enabled())fifo_load_sprite_data(1);

            FIFO.cur_state = F_SLEEP;
            break;
        case F_SLEEP:
            FIFO.cur_state = F_PUSH;
            break;
        case F_PUSH:
            if(fifo_push_pixel()) {
                FIFO.cur_state = F_TILE;
                if(window_enabled()) FIFO.window_pixel_size++;
            }
            break;
    }
}


void fifo_proc(){


    FIFO.map_x = FIFO.fetch_x + PPU->LCD->x_scroll;
    FIFO.map_y = PPU->LCD->ly + PPU->LCD->y_scroll;
    //if(window_enabled() && window_in_x() && window_in_y()) FIFO.tile_y = ((PPU->LCD->ly) % 8) *2;
    FIFO.tile_y = ( (PPU->LCD->ly + PPU->LCD->y_scroll) % 8) *2;

    fifo_fetch();

    fifo_draw_pixel();

}

void fifo_window_start(){
    FIFO.window_pixel_size = 0;
}

void fifo_reset(){
    while(!FIFO.fifo_pipeline.empty()) fifo_pop();
}