#include "ppu.h"
#include "emu.h"

uint8_t ppu::oam_read(uint16_t addr) {
    if(addr >= 0xFE00) addr -= 0xFE00;

    uint8_t *p = (uint8_t *)oam_ram;
    return p[addr];
}

void ppu::oam_write(uint16_t addr, uint8_t val){
    if(addr >= 0xFE00) addr -= 0xFE00;

    uint8_t *p = (uint8_t *)oam_ram;
    p[addr] = val;
    printf("addr: %04X val: %02X\n",addr,val);
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
    if(!dma_active) return;

    if(dma_delay){
        dma_delay--;
        return;
    }
    printf("oam_addr: %04X\n",(dma_val * 0x100) + dma_byte);
    oam_write(dma_byte, bus_read((dma_val * 0x100) + dma_byte));
    dma_byte++;
    dma_active = dma_byte < 0xA0;

    if(!dma_active){
        printf("yessir\n");
        SDL_Delay(2000);
    }
}






