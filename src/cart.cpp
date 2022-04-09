#include "cart.h"
#include <cstdio>
#include <cstdlib>


static char filename[1024];
static uint32_t rom_size;
static uint8_t *rom_data;


//static cart_context ctx;

static const char *ROM_TYPES[] = {
        "ROM ONLY",
        "MBC1",
        "MBC1+RAM",
        "MBC1+RAM+BATTERY",
        "0x04 ???",
        "MBC2",
        "MBC2+BATTERY",
        "0x07 ???",
        "ROM+RAM 1",
        "ROM+RAM+BATTERY 1",
        "0x0A ???",
        "MMM01",
        "MMM01+RAM",
        "MMM01+RAM+BATTERY",
        "0x0E ???",
        "MBC3+TIMER+BATTERY",
        "MBC3+TIMER+RAM+BATTERY 2",
        "MBC3",
        "MBC3+RAM 2",
        "MBC3+RAM+BATTERY 2",
        "0x14 ???",
        "0x15 ???",
        "0x16 ???",
        "0x17 ???",
        "0x18 ???",
        "MBC5",
        "MBC5+RAM",
        "MBC5+RAM+BATTERY",
        "MBC5+RUMBLE",
        "MBC5+RUMBLE+RAM",
        "MBC5+RUMBLE+RAM+BATTERY",
        "0x1F ???",
        "MBC6",
        "0x21 ???",
        "MBC7+SENSOR+RUMBLE+RAM+BATTERY",
};

bool cart_load(char *cart) {
    snprintf(filename, sizeof(filename), "%s", cart);

    FILE *fp = fopen(cart, "r");

    if (!fp) {
        printf("Failed to open: %s\n", cart);
        return false;
    }

    fseek(fp, 0, SEEK_END);
    rom_size = ftell(fp);

    rewind(fp);

    rom_data = (uint8_t *)(malloc(rom_size));
    fread(rom_data, rom_size, 1, fp);
    fclose(fp);



    printf("Cartridge Loaded:\n");

    return true;
}

uint8_t cart_read(uint16_t addr) {
    return rom_data[addr];
}

void cart_write(uint16_t addr, uint8_t val) {

}