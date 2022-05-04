#include "cart.h"

uint8_t cart::read(uint16_t addr) {
    return rom_data[addr];
}

void cart::write(uint16_t addr, uint8_t val) {

}