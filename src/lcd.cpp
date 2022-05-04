#include <cstdio>
#include "lcd.h"

uint32_t colors_default[4] = {0xFFFFFFFF, 0xFFAAAAAA, 0xFF555555, 0xFF000000};

static lcd LCD;

lcd * get_lcd(){
    return &LCD;
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
    //printf("lcdc: %i\n",LCD.stat);
    switch(addr & 0xFF){
        case 0x40: return LCD.lcdc;
        case 0x41: return LCD.stat;
        case 0x42: return LCD.y_scroll;
        case 0x43: return LCD.x_scroll;
        case 0x44: return LCD.ly;
        case 0x45: return LCD.lyc;
        case 0x46: return LCD.oam;
        case 0x47: return LCD.bg_palette;
        case 0x48: return LCD.palette_1;
        case 0x49: return LCD.palette_2;
        case 0x4A: return LCD.y_win;
        case 0x4B: return LCD.x_win;
    }
}

void lcd_write(uint16_t addr, uint8_t val) {
    //printf("lcdc: %i\n",LCD.stat);
    switch(addr & 0xFF){
        case 0x40: LCD.lcdc = val; break;
        case 0x41: LCD.stat = val; break;
        case 0x42: LCD.y_scroll = val; break;
        case 0x43: LCD.x_scroll = val; break;
        case 0x44: LCD.ly = val; break;
        case 0x45: LCD.lyc = val; break;
        case 0x46: LCD.oam = val; break;
        case 0x47: update_palette(val,0); break;
        case 0x48: update_palette(val & 0b11111100,1); break;
        case 0x49: update_palette(val & 0b11111100,2); break;
        case 0x4A: LCD.y_win = val; break;
        case 0x4B: LCD.x_win = val; break;
    }
}

void lcds_set(uint8_t val) {
    LCD.stat &= 0b11111100;
    LCD.stat |= val;
}

lcd_mode get_mode(){
    return (lcd_mode)(LCD.stat & 0b11);
}
