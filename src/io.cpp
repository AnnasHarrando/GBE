#include "io.h"
#include "emu.h"
#include "screen.h"
#include "audio.h"

static ppu *ppu;
static cpu *cpu;
static timer *timer;
static lcd *lcd;
static audio *audio;


void io_init(){
    cpu = get_cpu();
    ppu = get_ppu();
    timer = get_timer();
    lcd = get_lcd();
    audio = get_audio();
}

uint8_t serial_data[2] = {0};

uint8_t io_read(uint16_t addr) {
    if(addr == 0xFF00) return get_button_press();
    if(addr == 0xFF01) return serial_data[0];
    if(addr == 0xFF02) return serial_data[1];
    if((addr >= 0xFF04) && (addr <= 0xFF07)) return timer->read(addr);
    if(addr == 0xFF0F) return cpu->int_flags;
    if((addr >= 0xFF10) && (addr <= 0xFF3F)) return audio->read(addr);
    if((addr >= 0xFF40) && (addr <= 0xFF4B)) return lcd->read(addr);

    return 0;
}

void io_write(uint16_t addr, uint8_t val) {
    if(addr == 0xFF00) set_button_type(val);
    if(addr == 0xFF01) serial_data[0] = val;
    if(addr == 0xFF02) serial_data[1] = val;
    if((addr >= 0xFF04) && (addr <= 0xFF07)) timer->write(addr,val);
    if(addr == 0xFF0F) cpu->int_flags = val;
    if((addr >= 0xFF10) && (addr <= 0xFF3F)) audio->write(addr,val);
    if((addr >= 0xFF40) && (addr <= 0xFF4B)) lcd->write(addr,val);
    if(addr == 0xFF46) ppu->dma_start(val);

}
