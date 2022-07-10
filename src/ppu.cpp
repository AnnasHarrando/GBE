#include "ppu.h"
#include "emu.h"
#include "ppu_fifo.h"

static lcd *lcd;
static ppu_fifo *ppu_fifo;
static cpu *cpu;

void ppu_init(){
    lcd = get_lcd();
    ppu_fifo = get_ppu_fifo();
    cpu = get_cpu();
}

uint8_t ppu::oam_read(uint16_t addr) {
    if(addr >= 0xFE00) addr -= 0xFE00;

    uint8_t *p = (uint8_t *)oam_ram;
    return p[addr];
}

void ppu::oam_write(uint16_t addr, uint8_t val){
    if(addr >= 0xFE00) addr -= 0xFE00;

    uint8_t *p = (uint8_t *)oam_ram;
    p[addr] = val;
}

uint8_t ppu::vram_read(uint16_t addr) {
    return vram[addr - 0x8000];
}

void ppu::vram_write(uint16_t addr, uint8_t val) {
    vram[addr - 0x8000] = val;
}

void ppu::dma_start(uint8_t val){
    dma_val = val;
    dma_active = true;
    dma_byte = 0;
    dma_delay = 2;
}

void ppu::dma_tick(){

    if(dma_delay){
        dma_delay--;
        return;
    }
    oam_write(dma_byte, bus_read((dma_val * 0x100) + dma_byte));
    dma_byte++;
    dma_active = dma_byte < 0xA0;


}

void ppu::tick() {
    dots++;

    switch(lcd->get_mode()) {
        case MODE_OAM:
            oam_mode();
            break;
        case MODE_DRAW:
            draw_mode();
            break;
        case MODE_VBLANK:
            vblank_mode();
            break;
        case MODE_HBLANK:
            hblank_mode();
            break;
    }
}

static const int LCD_STAT = 2;
static const int VBLANK_STAT = 1;

void ppu::inc_ly(){

    if(lcd->window_enabled() && lcd->ly >= lcd->y_win && lcd->y_win < 144) window_line++;

    lcd->ly++;

    if(lcd->ly == lcd->lyc){
        lcd->stat |= (1 << 2);
        if(lcd->stat_int(STAT_LYC)) cpu->get_interrupt(LCD_STAT);
    }
    else lcd->stat &= ~(1 << 2);
}

void ppu::oam_mode(){
    if(dots >= 80){
        lcd->lcds_set(MODE_DRAW);
        ppu_fifo->set_draw();
    }
    if(dots == 79) ppu_fifo->load_sprites();
}

void ppu::draw_mode(){
    ppu_fifo->proc();

    if(ppu_fifo->drawing_stopped()) {
        ppu_fifo->reset();
        ppu_fifo->obj_drawn = 0;
        lcd->lcds_set(MODE_HBLANK);

        if (lcd->stat_int(STAT_HBLANK)) cpu->get_interrupt(LCD_STAT);


    }
}

void ppu::vblank_mode(){
    if(dots >= 456){
        inc_ly();

        if(lcd->ly >= 154){
            lcd->lcds_set(MODE_OAM);
            lcd->ly = 0;
            window_line = 0;
            if(lcd->stat_int(STAT_OAM)) cpu->get_interrupt(LCD_STAT);
        }

        dots = 0;
    }
}

void ppu::hblank_mode(){
    if(dots >= 456){
        inc_ly();

        if(lcd->ly >= 144){
            lcd->lcds_set(MODE_VBLANK);
            cpu->get_interrupt(VBLANK_STAT);

            if(lcd->stat_int(STAT_VBLANK)) cpu->get_interrupt(LCD_STAT);

            while(SDL_GetQueuedAudioSize(get_dev()) > 0){

            }
        }
        else {
            lcd->lcds_set(MODE_OAM);
            if(lcd->stat_int(STAT_OAM)) cpu->get_interrupt(LCD_STAT);
        }

        dots = 0;
    }
}








