#include "emu.h"
#include "cpu.h"
#include "cart.h"
#include "ram.h"
#include "io.h"
#include <iostream>

using namespace std;

cpu cpu;
cart cart;
ram ram;
io io;
bool running = true;

char dbg_msg[1024];
uint16_t msg_size = 0;

void dbg_update() {
    if (bus_read(0xFF02) == 0x81) {
        uint8_t c = bus_read(0xFF01);
        printf("adding to dbg\n");
        printf("%i\n",c);
        dbg_msg[msg_size] = c;
        msg_size += 1;
        bus_write(0xFF02, 0);
    }
    printf("DBG: %s size:%d\n", dbg_msg,msg_size);

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
    }
}

uint8_t bus_read(uint16_t addr) {
    if (addr < 0x8000) {
        //ROM Data
        return cart.cart_read(addr);
    }
    else if (addr < 0xA000) {
        //Char/Map Data
        return 0;
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
        return 0;
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
    if (addr < 0x8000) {
        //ROM Data
        cart.cart_write(addr, val);
    }
    else if (addr < 0xA000) {
        //Char/Map Data
    }
    else if (addr < 0xC000) {
        //EXT-RAM
        cart.cart_write(addr, val);
    }
    else if (addr < 0xE000) {
        //WRAM
        ram.wram_write(addr, val);
    }
    else if (addr < 0xFE00) {
        //reserved echo ram
    }
    else if (addr < 0xFEA0) {
        //OAM
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
