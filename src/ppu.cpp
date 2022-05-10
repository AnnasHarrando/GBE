#include "ppu.h"
#include "emu.h"
#include "ppu_fifo.h"



uint8_t ppu::oam_read(uint16_t addr) {
    if(addr >= 0xFE00) addr -= 0xFE00;

    uint8_t *p = (uint8_t *)oam_ram;
    return p[addr];
}

void ppu::oam_write(uint16_t addr, uint8_t val){
    if(addr >= 0xFE00) addr -= 0xFE00;

    uint8_t *p = (uint8_t *)oam_ram;
    p[addr] = val;
    //printf("addr: %04X val: %02X\n",addr,val);
}

uint8_t ppu::vram_read(uint16_t addr) {
    return vram[addr - 0x8000];
}

void ppu::vram_write(uint16_t addr, uint8_t val) {
    vram[addr - 0x8000] = val;
    if(addr >= 0x9800) {
        //printf("vram\n");
        //SDL_Delay(5000);
    }
}

void ppu::dma_start(uint8_t val){
    dma_val = val;
    dma_active = true;
    dma_byte = 0;
    dma_delay = 2;
}

void ppu::dma_tick(){
    if(!dma_active) return;

    if(dma_delay){
        dma_delay--;
        return;
    }
    oam_write(dma_byte, bus_read((dma_val * 0x100) + dma_byte));
    dma_byte++;
    dma_active = dma_byte < 0xA0;

    if(!dma_active){
    }
}

void ppu::tick() {
    dots++;
    //printf("%i\n",get_mode());
    switch(get_mode()) {
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

uint32_t fps = 1000/60;
unsigned long prev_frametime = 0;
unsigned long start_time = 0;
unsigned long frame_count = 0;


void ppu::inc_ly(){
    LCD->ly++;

    if(LCD->ly == LCD->lyc){
        LCD->stat |= (1 << 2);
        if(stat_int(STAT_LYC)) get_interrupt(LCD_STAT);
    }
    else LCD->stat &= ~(1 << 2);
}

void ppu::oam_mode(){
    if(dots >= 80){
        lcds_set(MODE_DRAW);
        fifo_set_draw();
    }
    if(dots == 1) fifo_load_sprites(oam_ram);
}

void ppu::draw_mode(){
    fifo_proc();
    if(drawing_stopped()){
        fifo_reset();
        lcds_set(MODE_HBLANK);

        if(stat_int(STAT_HBLANK)) get_interrupt(LCD_STAT);
    }
}

void ppu::vblank_mode(){
    if(dots >= 456){
        inc_ly();
        if(LCD->ly >= 154){
            lcds_set(MODE_OAM);
            LCD->ly = 0;
        }
        dots = 0;
    }
}

void ppu::hblank_mode(){
    if(dots >= 456){
        inc_ly();

        if(LCD->ly >= 144){
            lcds_set(MODE_VBLANK);
            get_interrupt(VBLANK_STAT);

            if(stat_int(STAT_VBLANK)) get_interrupt(LCD_STAT);

            cur_frame++;

            uint32_t end_time = SDL_GetTicks();
            if(end_time - prev_frametime < fps){
                SDL_Delay(fps-(end_time - prev_frametime));
            }

            if (end_time - start_time >= 1000) {
                uint32_t temp = frame_count;
                start_time = end_time;
                frame_count = 0;

                printf("FPS: %d\n", temp);
            }

            frame_count++;
            prev_frametime = SDL_GetTicks();
            //printf("x: %i y: %i\n",oam_ram[0].x,oam_ram[0].y);
        }
        else lcds_set(MODE_OAM);

        dots = 0;
    }
}








