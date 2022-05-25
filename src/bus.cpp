#include "bus.h"
#include "emu.h"
#include "cart.h"

static cart *cart;
static io *io;
static ram *ram;
static ppu *ppu;
static cpu *cpu;

void bus_init(){
    cart = get_cart();
    io = get_io();
    ram = get_ram();
    ppu = get_ppu();
    cpu = get_cpu();
}

uint8_t bus::read(uint16_t addr) {
        if (addr < 0x4000) {
            return cart->read(addr);
        }

        if (addr < 0x8000) {
            return cart->rom_bank_read(addr);
        }

        else if (addr < 0xA000) {
            //if(oam_mode()) return 0xFF;
            return ppu->vram_read(addr);
        }

        else if (addr < 0xC000) {
            return cart->ram_read(addr);
        }

        else if (addr < 0xE000) {
            return ram->wram_read(addr);
        }

        else if (addr < 0xFE00) {
            return 0;
        }

        else if (addr < 0xFEA0) {
            if(ppu->dma_active) return 0xFF;
            else return ppu->oam_read(addr);
        }

        else if (addr < 0xFF00) {
            return 0;
        }

        else if (addr < 0xFF80) {
            return io->read(addr);
        }

        else if(addr < 0xFFFF){
            return ram->hram_read(addr);
        }

        else if(addr == 0xFFFF){
            return cpu->get_ie_register();
        }

        printf("No bus addr %04X", addr);
        exit(-5);

}

void bus::write(uint16_t addr, uint8_t val) {
    if (addr < 0x4000) {
        cart->write(addr, val);
    }

    if (addr < 0x8000) {
        cart->rom_bank_write(addr, val);
    }

    else if (addr < 0xA000) {
        //if(oam_mode()) return;
        ppu->vram_write(addr, val);
    }

    else if (addr < 0xC000) {
        cart->ram_write(addr, val);
    }

    else if (addr < 0xE000) {
        ram->wram_write(addr, val);
    }

    else if (addr < 0xFE00) {

    }

    else if (addr < 0xFEA0) {
        if(ppu->dma_active) return;
        ppu->oam_write(addr, val);
    }

    else if (addr < 0xFF00) {

    }

    else if (addr < 0xFF80) {
        io->write(addr, val);
    }

    else if (addr < 0xFFFF) {
        ram->hram_write(addr, val);
    }

    else if(addr == 0xFFFF){
        cpu->set_ie_register(val);
    }
}
