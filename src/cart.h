#include <cstdint>
#include <cstdio>
#include <cstdlib>

class cart{
public:
    char filename[1024];
    uint32_t rom_size;
    uint8_t *rom_data;

    uint8_t cart_type;

    uint8_t *cur_rom_bank;
    uint8_t *rom_banks[16];



    void cart_load(char *cart){
        snprintf(filename, sizeof(filename), "%s", cart);

        FILE *fp = fopen(cart, "r");

        if (!fp) {
            printf("Failed to open: %s\n", cart);
            exit(-2);
        }

        fseek(fp, 0, SEEK_END);
        rom_size = ftell(fp);

        rewind(fp);

        rom_data = (uint8_t *)(malloc(rom_size));
        fread(rom_data, rom_size, 1, fp);
        fclose(fp);

        printf("Cartridge Loaded:\n");

        cart_type = rom_data[0x0147];
        cur_rom_bank = rom_data + 0x4000;
    };

    uint8_t read(uint16_t address);
    void write(uint16_t address, uint8_t value);

    uint8_t rom_bank_read(uint16_t address);
    void rom_bank_write(uint16_t address, uint8_t value);

    uint8_t ram_read(uint16_t address);
    void ram_write(uint16_t address, uint8_t value);
};
