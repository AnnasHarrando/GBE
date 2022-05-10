#ifndef LCD_H
#define LCD_H
#include <cstdint>

typedef struct{
    uint8_t lcdc = 0x91;
    uint8_t stat = 0b10;
    uint8_t x_scroll = 0;
    uint8_t y_scroll = 0;
    uint8_t ly = 0;
    uint8_t lyc = 0;
    uint8_t oam;
    uint8_t bg_palette = 0xFC;
    uint8_t palette_1 = 0xFF;
    uint8_t palette_2 = 0xFF;
    uint8_t x_win = 0;
    uint8_t y_win = 0;

    uint32_t bg_color[4] = {0xFFFFFFFF, 0xFFAAAAAA, 0xFF555555, 0xFF000000};
    uint32_t sp1_color[4] = {0xFFFFFFFF, 0xFFAAAAAA, 0xFF555555, 0xFF000000};
    uint32_t sp2_color[4] = {0xFFFFFFFF, 0xFFAAAAAA, 0xFF555555, 0xFF000000};
}lcd;

lcd * get_lcd();

typedef enum {
    MODE_HBLANK = 0,
    MODE_VBLANK = 1,
    MODE_OAM = 2,
    MODE_DRAW = 3
} lcd_mode;

typedef enum {
    STAT_HBLANK,
    STAT_VBLANK,
    STAT_OAM,
    STAT_LYC
} stat_mode;

uint8_t get_sprite_height();
uint8_t lcd_read(uint16_t address);
void lcd_write(uint16_t address, uint8_t value);
void update_palette(uint8_t val, uint8_t palette);
void lcds_set(uint8_t val);
bool stat_int(stat_mode mode);
bool BGW_enabled();
bool OBJ_enabled();
uint16_t BGW_data_loc();
uint16_t BG_map_loc();
lcd_mode get_mode();

#endif