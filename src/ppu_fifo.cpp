#include "emu.h"
#include <algorithm>

static ppu *PPU;
static lcd *lcd;
static bus *bus;

void fifo_init(){
    PPU = get_ppu();
    lcd = get_lcd();
    bus = get_bus();
}

bool toggle = false;

void ppu_fifo::proc(){


    map_x = fetch_x + lcd->x_scroll;
    map_y = lcd->ly + lcd->y_scroll;
    if(lcd->window_enabled() && window_in_x() && window_in_y()) tile_y = ((lcd->ly) % 8)*2;
    else tile_y = ( (lcd->ly + lcd->y_scroll) % 8) *2;

    if(toggle) fetch();
    toggle = ~toggle;

    draw_pixel();

}

void ppu_fifo::fetch(){
    switch(cur_state){
        case F_TILE:
            PPU->fetch_entry_size = 0;

            if(lcd->BGW_enabled()){
                bgw_fetch_data[0] = bus->read(lcd->BG_map_loc() + (map_x / 8) + ((map_y / 8) * 32));

                if(lcd->BGW_data_loc() == 0x8800) bgw_fetch_data[0] += 128;

                load_window();
            }

            if(lcd->OBJ_enabled() && PPU->line_sprites_size > 0) load_sprite_tile();

            cur_state = F_DATA0;
            fetch_x += 8;
            break;
        case F_DATA0:
            bgw_fetch_data[1] = bus->read(lcd->BGW_data_loc() + (bgw_fetch_data[0] * 16) + tile_y);

            load_sprite_data(0);

            cur_state = F_DATA1;
            break;
        case F_DATA1:
            bgw_fetch_data[2] = bus->read(lcd->BGW_data_loc() + (bgw_fetch_data[0] * 16) + tile_y + 1);

            load_sprite_data(1);

            cur_state = F_SLEEP;
            break;
        case F_SLEEP:
            cur_state = F_PUSH;
            break;
        case F_PUSH:
            if(push_pixel()) {
                cur_state = F_TILE;
            }
            break;
    }
}

bool ppu_fifo::push_pixel(){
    if(fifo_pipeline.size() > 8) return false;


    for (int i=0; i<8; i++) {
        uint32_t color;
        int bit = 7 - i;

        uint8_t low = !!(bgw_fetch_data[1] & (1 << bit));
        uint8_t high = !!(bgw_fetch_data[2] & (1 << bit)) << 1;

        color = lcd->bg_color[high | low];

        if(!lcd->BGW_enabled()) color = lcd->bg_color[0];

        if(lcd->OBJ_enabled()) color = get_sprite(bit, color, high | low);

        pixel temp = {color,lcd->window_enabled() && window_in_x() && window_in_y()};

        push(temp);
        fifo_x++;

    }

    return true;
}

void ppu_fifo::draw_pixel(){
    if(fifo_pipeline.size() > 8){
        pixel temp = pop();

        if(line_x >= (lcd->x_scroll % 8) || temp.isWindow){
            PPU->buffer[pushed_x + (lcd->ly * 160)] = temp.color;
            pushed_x++;
        }

        line_x++;
    }
}

bool compare(oam left,oam right){ return left.x < right.x;}

void ppu_fifo::load_sprites() {
    memset(PPU->line_sprites,0,10);
    PPU->line_sprites_size = 0;

    uint8_t height = lcd->get_sprite_height();

    for (int i = 0; i < 40; i++){
        if(PPU->line_sprites_size == 10) break;

        oam sprite = PPU->oam_ram[i];

        if(sprite.x > 0 && (sprite.y <= lcd->ly + 16 && sprite.y + height > lcd->ly + 16)){
            PPU->line_sprites[PPU->line_sprites_size] = sprite;
            PPU->line_sprites_size++;
        }
    }

    std::sort(PPU->line_sprites,PPU->line_sprites+PPU->line_sprites_size, compare);
}

uint32_t ppu_fifo::get_sprite(uint8_t bit, uint32_t color, uint8_t bg){

    uint32_t temp = color;
    for(int i=0; i < PPU->fetch_entry_size; i++){
        int x = PPU->fetch_entry[i].x - 8 + (lcd->x_scroll % 8);
        int offset = fifo_x - x;

        if(x+8 >= fifo_x && (offset >= 0 && offset <= 7)){
            if(PPU->fetch_entry[i].x_flip) bit = offset;
            else bit = (7 - offset);

            uint8_t low = !!(fetch_entry_data[i * 2] & (1 << bit));
            uint8_t high = !!(fetch_entry_data[(i * 2) + 1] & (1 << bit)) << 1;

            if(high | low){
                if(!PPU->fetch_entry[i].bgp || bg == 0){
                    temp = (PPU->fetch_entry[i].pn) ?
                           lcd->sp2_color[high | low] : lcd->sp1_color[high | low];
                }
                break;
            }
        }
    }

    return temp;
}

void ppu_fifo::load_sprite_tile(){
    for(int i = 0; i < PPU->line_sprites_size; i++){

        int x = PPU->line_sprites[i].x - 8 + (lcd->x_scroll % 8);

        if((x >= fetch_x && x < fetch_x + 8) || (x+8 >= fetch_x && x+8 < fetch_x + 8)){
            PPU->fetch_entry[PPU->fetch_entry_size] = PPU->line_sprites[i];
            PPU->fetch_entry_size++;
        }

        if(PPU->fetch_entry_size >=3) break;
    }
}

void ppu_fifo::load_sprite_data(uint8_t val){
    uint8_t height = lcd->get_sprite_height();

    for(int i=0; i < PPU->fetch_entry_size; i++){
        uint8_t y = (lcd->ly + 16 - PPU->fetch_entry[i].y) * 2;
        if(PPU->fetch_entry[i].y_flip) y = height*2 - 2 - y;

        uint8_t index = PPU->fetch_entry[i].tile;
        if(height == 16) index &= ~1;

        fetch_entry_data[i*2 + val] = bus->read(0x8000 + index*16 + y + val);
    }
}

void ppu_fifo::load_window(){
    if(lcd->window_enabled() && window_in_x() && window_in_y()) {

        uint8_t y = PPU->window_line / 8;
        bgw_fetch_data[0] = bus->read(lcd->win_map_loc() + ((fetch_x + 7 - lcd->x_win) / 8) + (y * 32));

        if (lcd->BGW_data_loc() == 0x8800) bgw_fetch_data[0] += 128;
    }

}

bool ppu_fifo::window_in_x(){
    return fetch_x + 7 >= lcd->x_win && fetch_x + 7 < lcd->x_win + 144 + 14;
}

bool ppu_fifo::window_in_y(){
    return lcd->ly >= lcd->y_win && lcd->ly < lcd->y_win + 160;
}

void ppu_fifo::set_draw(){
    cur_state = F_TILE;
    line_x = 0;
    fetch_x = 0;
    pushed_x = 0;
    fifo_x = 0;
}

bool ppu_fifo::drawing_stopped(){
    return pushed_x >= 160;
}

pixel ppu_fifo::pop(){
    pixel temp = fifo_pipeline.front();
    fifo_pipeline.pop();
    return temp;
}

void ppu_fifo::push(pixel val){
    fifo_pipeline.push(val);
}

void ppu_fifo::reset(){
    while(!fifo_pipeline.empty()) pop();
}