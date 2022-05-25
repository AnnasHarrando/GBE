#include "lcd.h"

uint32_t colors_default[4] = {0xFFFFFFFF, 0xFFAAAAAA, 0xFF555555, 0xFF000000};


uint8_t lcd::read(uint16_t addr) {
    switch(addr){
        case 0xFF40: return lcdc;
        case 0xFF41: return stat;
        case 0xFF42: return y_scroll;
        case 0xFF43: return x_scroll;
        case 0xFF44: return ly;
        case 0xFF45: return lyc;
        case 0xFF46: return oam;
        case 0xFF47: return bg_palette;
        case 0xFF48: return palette_1;
        case 0xFF49: return palette_2;
        case 0xFF4A: return y_win;
        case 0xFF4B: return x_win;
    }
}

void lcd::write(uint16_t addr, uint8_t val) {
    switch(addr){
        case 0xFF40: lcdc = val; break;
        case 0xFF41: stat = val; break;
        case 0xFF42: y_scroll = val; break;
        case 0xFF43: x_scroll = val; break;
        case 0xFF44: ly = val; break;
        case 0xFF45: lyc = val; break;
        case 0xFF46: oam = val; break;
        case 0xFF47: update_palette(val,0); break;
        case 0xFF48: update_palette(val & 0b11111100,1); break;
        case 0xFF49: update_palette(val & 0b11111100,2); break;
        case 0xFF4A: y_win = val; break;
        case 0xFF4B: x_win = val; break;
    }
}

uint8_t lcd::get_sprite_height() {
    if((lcdc >> 2) & 0x1) return 16;
    else return 8;
}

bool lcd::stat_int(stat_mode mode){
    switch (mode) {
        case STAT_HBLANK:
            return stat & (1 << 3);
        case STAT_VBLANK:
            return stat & (1 << 4);
        case STAT_OAM:
            return stat & (1 << 5);
        case STAT_LYC:
            return stat & (1 << 6);
    }
}

void lcd::update_palette(uint8_t val, uint8_t palette){
    switch(palette){
        case 0:
            bg_color[0] = colors_default[val & 0b11];
            bg_color[1] = colors_default[(val >> 2) & 0b11];
            bg_color[2] = colors_default[(val >> 4) & 0b11];
            bg_color[3] = colors_default[(val >> 6) & 0b11];
            break;
        case 1:
            sp1_color[0] = colors_default[val & 0b11];
            sp1_color[1] = colors_default[(val >> 2) & 0b11];
            sp1_color[2] = colors_default[(val >> 4) & 0b11];
            sp1_color[3] = colors_default[(val >> 6) & 0b11];
            break;
        case 2:
            sp2_color[0] = colors_default[val & 0b11];
            sp2_color[1] = colors_default[(val >> 2) & 0b11];
            sp2_color[2] = colors_default[(val >> 4) & 0b11];
            sp2_color[3] = colors_default[(val >> 6) & 0b11];
            break;
    }
}

void lcd::lcds_set(uint8_t val) {
    stat &= 0b11111100;
    stat |= val;
}

lcd_mode lcd::get_mode(){
    return (lcd_mode)(stat & 0b11);
}

bool lcd::BGW_enabled(){
    return lcdc & 1;
}

bool lcd::OBJ_enabled(){
    return (lcdc & 0b10);
}

uint16_t lcd::BGW_data_loc(){
    if(lcdc & (1 << 4)) return 0x8000;
    else return 0x8800;
}
uint16_t lcd::BG_map_loc(){
    if(lcdc & (1 << 3)) return 0x9C00;
    else return 0x9800;
}

bool lcd::window_enabled(){
    return ((lcdc >> 5) & 1) && x_win >= 0 && x_win <= 166 && y_win >= 0 && y_win < 144;
}

uint16_t lcd::win_map_loc(){
    if((lcdc >> 6) & 0x1) return 0x9C00;
    else return 0x9800;
}

