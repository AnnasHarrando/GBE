#include "emu.h"
#include <algorithm>

static ppu *PPU;
static lcd *lcd;

void fifo_init(){
    PPU = get_ppu();
    lcd = get_lcd();
}

bool toggle = false;

void ppu_fifo::proc(){

    if(toggle) fetch();
    toggle = ~toggle;

    draw_pixel();

}

void ppu_fifo::fetch(){
    switch(cur_state){
        case F_TILE:
            if(lcd->BGW_enabled()){
                map_x = fetch_x + lcd->x_scroll;
                map_y = lcd->ly + lcd->y_scroll;

                bgw_fetch_index = bus_read(lcd->BG_map_loc() + (map_x / 8) + ((map_y / 8) * 32));

                if(lcd->window_enabled() && window_in_x() && window_in_y()) {
                    uint8_t y = PPU->window_line / 8;
                    bgw_fetch_index = bus_read(lcd->win_map_loc() + ((fetch_x + 7 - lcd->x_win) / 8) + (y * 32));
                }

                if(lcd->BGW_data_loc() == 0x8800) {
                    bgw_fetch_index += 128;
                }
            }

            cur_state = F_DATA0;
            fetch_x += 8;
            break;
        case F_DATA0:
            if(lcd->window_enabled() && window_in_x() && window_in_y()) tile_y = ((lcd->ly) % 8)*2;
            else tile_y = ( (lcd->ly + lcd->y_scroll) % 8) *2;

            bgw_fetch_data[0] = bus_read(lcd->BGW_data_loc() + (bgw_fetch_index * 16) + tile_y);

            cur_state = F_DATA1;
            break;
        case F_DATA1:
            bgw_fetch_data[1] = bus_read(lcd->BGW_data_loc() + (bgw_fetch_index * 16) + tile_y + 1);

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
        int bit = 7 - i;

        uint8_t low = !!(bgw_fetch_data[0] & (1 << bit));
        uint8_t high = !!(bgw_fetch_data[1] & (1 << bit)) << 1;

        pixel temp;
        if(lcd->window_enabled() && window_in_y() && fetch_x - 1 >= lcd->x_win) temp = {static_cast<uint8_t>((high | low)), WINDOW};
        else temp = {static_cast<uint8_t>((high | low)), BG};

        if(!lcd->BGW_enabled()) temp.colorID = 0;

        push(temp);
        fifo_x++;
    }
    return true;
}

void ppu_fifo::draw_pixel(){
    if(fifo_pipeline.size() > 8){
        pixel temp = pop();

        if(line_x >= (lcd->x_scroll % 8) || temp.type == WINDOW){
            if(temp.type == BG) PPU->buffer[pushed_x + (lcd->ly * 160)] = lcd->bg_color[temp.colorID];
            else if(temp.type == WINDOW) PPU->buffer[pushed_x + (lcd->ly * 160)] = lcd->bg_color[temp.colorID];
            else if(temp.type == OBJ1) PPU->buffer[pushed_x + (lcd->ly * 160)] = lcd->sp1_color[temp.colorID];
            else if(temp.type == OBJ2) PPU->buffer[pushed_x + (lcd->ly * 160)] = lcd->sp2_color[temp.colorID];

            pushed_x++;
            if(lcd->OBJ_enabled() && PPU->line_sprites[obj_drawn].x <= pushed_x + 8 && obj_drawn < PPU->line_sprites_size){
                load_sprite();
            }
        }
        else line_x++;
    }
}

void ppu_fifo::load_sprite() {
    uint8_t height = lcd->get_sprite_height();

    uint8_t y = (lcd->ly + 16 - PPU->line_sprites[obj_drawn].y) * 2;
    if(PPU->line_sprites[obj_drawn].y_flip) y = height * 2 - 2 - y;

    uint8_t index = PPU->line_sprites[obj_drawn].tile;
    if(height == 16) index &= ~1;

    uint8_t lowbyte = bus_read(0x8000 + index*16 + y );
    uint8_t highbyte = bus_read(0x8000 + index*16 + y + 1);

    uint8_t offset = 0;
    for(int i = 0; i < 8; i++){
        if(PPU->line_sprites[obj_drawn].x + i < 8) {
            offset++;
            continue;
        }
        uint8_t bit;
        if(PPU->line_sprites[obj_drawn].x_flip) bit = i;
        else bit = (7 - i);

        uint8_t low = !!(lowbyte & (1 << bit));
        uint8_t high = !!(highbyte & (1 << bit)) << 1;

        if((high || low) && fifo_pipeline[i-offset].type != OBJ1 && fifo_pipeline[i-offset].type != OBJ2){
            if(!PPU->line_sprites[obj_drawn].bgp || fifo_pipeline[i-offset].colorID == 0){
                if(PPU->line_sprites[obj_drawn].pn) fifo_pipeline[i-offset] = {static_cast<uint8_t>((high | low)), OBJ2};
                else fifo_pipeline[i-offset] = {static_cast<uint8_t>((high | low)),OBJ1};
            }
        }
    }
    obj_drawn++;
}

bool compare(oam left,oam right){ return left.x < right.x;}

void ppu_fifo::load_sprites() {
    memset(PPU->line_sprites,0,10*sizeof(oam));
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

    std::sort(PPU->line_sprites,PPU->line_sprites + PPU->line_sprites_size, compare);
}

bool ppu_fifo::window_in_x(){
    return fetch_x + 7 >= lcd->x_win;
}

bool ppu_fifo::window_in_y(){
    return lcd->ly >= lcd->y_win;
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
    fifo_pipeline.erase(fifo_pipeline.begin());
    return temp;
}

void ppu_fifo::push(pixel val){
    fifo_pipeline.push_back(val);
}

void ppu_fifo::reset(){
    while(!fifo_pipeline.empty()) pop();
}

