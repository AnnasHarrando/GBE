#include "emu.h"
#include "cpu.h"
#include "cart.h"
#include "ram.h"
#include "io.h"
#include "ppu.h"
#include <iostream>

using namespace std;

cpu cpu;
cart cart;
ram ram;
io io;
ppu ppu;
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

int start(int argc, char **argv) {
    cart.cart_load(argv[1]);
    init();
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
    while(running){
        running = ui_handle_events();
        update_dbg_window();

    }
}

uint8_t bus_read(uint16_t addr) {
    if (addr < 0x8000) {
        //ROM Data
        return cart.cart_read(addr);
    }
    else if (addr < 0xA000) {
        //Char/Map Data
        return ppu.vram_read(addr);
    }
    else if (addr < 0xC000) {
        //Cartridge RAM
        return cart.cart_read(addr);
    }
    else if (addr < 0xE000) {
        //WRAM (Working RAM)
        return ram.wram_read(addr);
    }
    else if (addr < 0xFE00) {
        //reserved echo ram...
        return 0;
    }
    else if (addr < 0xFEA0) {
        //OAM
        return ppu.oam_read(addr);
    }
    else if (addr < 0xFF00) {
        //reserved unusable...
        return 0;
    }
    else if (addr < 0xFF80) {
        //IO Registers...
        return io.io_read(addr);
    }
    else if(addr < 0xFFFF){
        return ram.hram_read(addr);
    }
    else if(addr == 0xFFFF){
        //  return
        return cpu.get_ie_register();
    }
    printf("No bus addr %04X", addr);
    exit(-5);
}

void bus_write(uint16_t addr, uint8_t val) {
    //printf("Writing to %04X\n\n", addr);
    if (addr < 0x8000) {
        //ROM Data
        cart.cart_write(addr, val);
    }
    else if (addr < 0xA000) {
        //Char/Map Data
        ppu.vram_write(addr, val);
    }
    else if (addr < 0xC000) {
        //EXT-RAM
        cart.cart_write(addr, val);
    }
    else if (addr < 0xE000) {
        //WRAM
        printf("wram write: %04X val:%02X\n",addr,val);
        ram.wram_write(addr, val);
    }
    else if (addr < 0xFE00) {
        //reserved echo ram
    }
    else if (addr < 0xFEA0) {
        //OAM
        if(ppu.dma_active) return;
        ppu.oam_write(addr, val);
    }
    else if (addr < 0xFF00) {
        //unusable reserved
    }
    else if (addr < 0xFF80) {
        //IO Registers...
        io.io_write(addr,val);
    }
    else if (addr < 0xFFFF) {
        ram.hram_write(addr, val);
    }
    else if(addr == 0xFFFF){
        //CPU SET ENABLE REGISTER
        cpu.set_ie_register(val);
    }
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

void cycles(uint8_t cycle){
    for(int i=0; i<cycle; i++) {
        for (int j = 0; j < 4; j++) {
            ticks++;
            tick();
        }
        ppu.dma_tick();
    }

}

void dma_start(uint8_t val){
    ppu.dma_start(val);
}