#include "emu.h"
#include "cpu.h"
#include "cart.h"
#include "ram.h"
#include "io.h"
#include "ppu.h"
#include "bus.h"
#include "ppu_fifo.h"

using namespace std;

cpu cpu;
cart cart;
ram ram;
io io;
ppu ppu;
bus bus;
bool running = true;

uint8_t ticks = 0;

char dbg_msg[1024];
uint16_t msg_size = 0;

void dbg_update() {
    if (bus_read(0xFF02) == 0x81) {
        uint8_t c = bus_read(0xFF01);
        dbg_msg[msg_size] = c;
        msg_size += 1;
        bus_write(0xFF02, 0);
        printf("DBG: %s size:%d\n", dbg_msg,msg_size);
    }

}



DWORD WINAPI cpu_run(LPVOID lpParameter)
{

    while(true){
        cpu.step();
        dbg_update();
    }
}

void fifo_init(){
    init(ppu);
}

int start(int argc, char **argv) {
    cart.cart_load(argv[1]);
    init();
    fifo_init();
    ticks = 0;

    HANDLE hThread = CreateThread(
            NULL,    // Thread attributes
            0,       // Stack size (0 = use default)
            cpu_run, // Thread start address
            NULL,    // Parameter to pass to the thread
            0,       // Creation flags
            NULL);

    if (hThread == NULL)
    {
        // Thread creation failed.
        // More details can be retrieved by calling GetLastError()
        printf("damn");
        return 1;
    }

    ui_init();
    while(true){
        running = ui_handle_events();
        update_screen(ppu.buffer);
        update_dbg_window();

    }
}

uint8_t emu_read(uint16_t addr, component comp) {
    switch(comp){
        case WRAM: return ram.wram_read(addr);
        case HRAM: return ram.hram_read(addr);
        case CART: return cart.read(addr);
        case IO: return io.read(addr);
        case OAM: return ppu.oam_read(addr);
        case VRAM: return ppu.vram_read(addr);
        default: break;
    }
}

void emu_write(uint16_t addr, uint8_t val, component comp) {
    switch(comp){
        case WRAM: ram.wram_write(addr,val); break;
        case HRAM: ram.hram_write(addr,val); break;
        case CART: cart.write(addr,val); break;
        case IO: io.write(addr,val); break;
        case OAM: ppu.oam_write(addr,val); break;
        case VRAM: ppu.vram_write(addr,val); break;
        default: break;
    }
}

void cycles(uint8_t cycle){
    for(int i=0; i<cycle; i++) {
        for (int j = 0; j < 4; j++) {
            ticks++;
            tick();
            ppu.tick();
        }
        ppu.dma_tick();
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