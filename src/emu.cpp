#include "emu.h"
#include "cpu.h"
#include "cart.h"
#include "ram.h"
#include "io.h"
#include "ppu.h"
#include "bus.h"
#include "ppu_fifo.h"
#include "audio.h"

using namespace std;

cpu cpu;
cart cart;
ram ram;
io io;
ppu ppu;
bus bus;


DWORD WINAPI cpu_run(LPVOID lpParameter)
{

    while(true){
        if(get_running()) cpu.step();
    }
}


void fifo_init(){
    init(&ppu);
}

bool oam_mode(){
    return get_mode() == MODE_DRAW;
}


uint8_t *get_oam_tiles(){
    uint8_t *temp = static_cast<uint8_t *>(malloc(40));
    for(int i=0; i<40; i++){
        temp[i] = ppu.oam_ram[i].tile;
    }
    return temp;
}

int start(int argc, char **argv) {
    cart.load_rom(argv[1]);

    inst_init();
    fifo_init();
    audio_init();

    HANDLE hThread = CreateThread(
            NULL,    // Thread attributes
            0,       // Stack size (0 = use default)
            cpu_run, // Thread start address
            NULL,    // Parameter to pass to the thread
            0,       // Creation flags
            NULL);

    if (hThread == NULL)
    {

        return 1;
    }

    ui_init();
    while(ui_handle_events()){
        update_screen(ppu.buffer);
#if false
        update_dbg_window();
        t_update_dbg_window();
        oam_update_dbg_window(get_oam_tiles());
#endif

    }
}

uint8_t emu_read(uint16_t addr, component comp) {
    switch(comp){
        case WRAM: return ram.wram_read(addr);
        case HRAM: return ram.hram_read(addr);
        case RAM_BANK: return cart.ram_read(addr);
        case CART: return cart.read(addr);
        case CART_BANK: return cart.rom_bank_read(addr);
        case IO: return io.read(addr);
        case OAM: return ppu.oam_read(addr);
        case VRAM: return ppu.vram_read(addr);
        default: break;
    }
}

void emu_write(uint16_t addr, uint8_t val, component comp) {
    switch(comp){
        case WRAM:ram.wram_write(addr,val); break;
        case HRAM: ram.hram_write(addr,val); break;
        case RAM_BANK: cart.ram_write(addr, val); break;
        case CART: cart.write(addr,val); break;
        case CART_BANK: cart.rom_bank_write(addr, val); break;
        case IO: io.write(addr,val); break;
        case OAM: ppu.oam_write(addr,val); break;
        case VRAM: ppu.vram_write(addr,val); break;
        default: break;
    }
}


void cycles(uint8_t cycle){
    for(int i=0; i<cycle; i++) {
        for (int j = 0; j < 4; j++) {
            tick();
            ppu.tick();
        }
        ppu.dma_tick();
    }
    for(int i=0; i<1; i++){
        //timing
    }
}

uint8_t bus_read(uint16_t addr){
    return bus.read(addr);
}

void bus_write(uint16_t addr, uint8_t val){
    bus.write(addr,val);
};

bool dma_active(){
    return ppu.dma_active;
}

void set_ie_register(uint8_t val){
    cpu.set_ie_register(val);
}

uint8_t get_ie_register(){
    return cpu.get_ie_register();
}

uint8_t get_int_flags(){
    return cpu.int_flags;
}

void set_int_flags(uint8_t val){
    cpu.int_flags = val;
}

void get_interrupt(uint8_t val){
    cpu.get_interrupt(val);
}

void dma_start(uint8_t val){
    ppu.dma_start(val);
}