#include "lcd.h"

uint32_t colors_default[4] = {0xFFFFFFFF, 0xFFAAAAAA, 0xFF555555, 0xFF000000};

static lcd LCD;

lcd * get_lcd(){
    return &LCD;
}

uint8_t get_sprite_height(){
    if((LCD.lcdc >> 2) & 0x1) return 16;
    else return 8;
}

bool stat_int(stat_mode mode){
    switch (mode) {
        case STAT_HBLANK:
            return LCD.stat & (1 << 3);
        case STAT_VBLANK:
            return LCD.stat & (1 << 4);
        case STAT_OAM:
            return LCD.stat & (1 << 5);
        case STAT_LYC:
            return LCD.stat & (1 << 6);
    }
}

void update_palette(uint8_t val, uint8_t palette){
    switch(palette){
        case 0:
            LCD.bg_color[0] = colors_default[val & 0b11];
            LCD.bg_color[1] = colors_default[(val >> 2) & 0b11];
            LCD.bg_color[2] = colors_default[(val >> 4) & 0b11];
            LCD.bg_color[3] = colors_default[(val >> 6) & 0b11];
            break;
        case 1:
            LCD.sp1_color[0] = colors_default[val & 0b11];
            LCD.sp1_color[1] = colors_default[(val >> 2) & 0b11];
            LCD.sp1_color[2] = colors_default[(val >> 4) & 0b11];
            LCD.sp1_color[3] = colors_default[(val >> 6) & 0b11];
            break;
        case 2:
            LCD.sp2_color[0] = colors_default[val & 0b11];
            LCD.sp2_color[1] = colors_default[(val >> 2) & 0b11];
            LCD.sp2_color[2] = colors_default[(val >> 4) & 0b11];
            LCD.sp2_color[3] = colors_default[(val >> 6) & 0b11];
            break;
    }
};


uint8_t lcd_read(uint16_t addr) {
    switch(addr){
        case 0xFF40: return LCD.lcdc;
        case 0xFF41: return LCD.stat;
        case 0xFF42: return LCD.y_scroll;
        case 0xFF43: return LCD.x_scroll;
        case 0xFF44: return LCD.ly;
        case 0xFF45: return LCD.lyc;
        case 0xFF46: return LCD.oam;
        case 0xFF47: return LCD.bg_palette;
        case 0xFF48: return LCD.palette_1;
        case 0xFF49: return LCD.palette_2;
        case 0xFF4A: return LCD.y_win;
        case 0xFF4B: return LCD.x_win;
    }
}

void lcd_write(uint16_t addr, uint8_t val) {
    switch(addr){
        case 0xFF40: LCD.lcdc = val; break;
        case 0xFF41: LCD.stat = val; break;
        case 0xFF42: LCD.y_scroll = val; break;
        case 0xFF43: LCD.x_scroll = val; break;
        case 0xFF44: LCD.ly = val; break;
        case 0xFF45: LCD.lyc = val; break;
        case 0xFF46: LCD.oam = val; break;
        case 0xFF47: update_palette(val,0); break;
        case 0xFF48: update_palette(val & 0b11111100,1); break;
        case 0xFF49: update_palette(val & 0b11111100,2); break;
        case 0xFF4A: LCD.y_win = val; break;
        case 0xFF4B: LCD.x_win = val; break;
    }
}

void lcds_set(uint8_t val) {
    LCD.stat &= 0b11111100;
    LCD.stat |= val;
}

lcd_mode get_mode(){
    return (lcd_mode)(LCD.stat & 0b11);
}

bool BGW_enabled(){
    return LCD.lcdc & 1;
}

bool OBJ_enabled(){
    return (LCD.lcdc & 0b10);
}


uint16_t BGW_data_loc(){
    if(LCD.lcdc & (1 << 4)) return 0x8000;
    else return 0x8800;
}
uint16_t BG_map_loc(){
    if(LCD.lcdc & (1 << 3)) return 0x9C00;
    else return 0x9800;
}

bool window_enabled(){
    return ((LCD.lcdc >> 5) & 1) && LCD.x_win >= 0 && LCD.x_win <= 166 && LCD.y_win >= 0
    && LCD.y_win < 144;
}

uint16_t win_map_loc(){
    if((LCD.lcdc >> 6) & 0x1) return 0x9C00;
    else return 0x9800;
}