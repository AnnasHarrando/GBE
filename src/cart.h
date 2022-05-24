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

    bool ram_bank_enabled = false;
    bool ram_enabled = false;
    uint8_t *cur_ram_bank;
    uint8_t cur_ram_bank_val;
    uint8_t *ram_banks;
    uint32_t ram_size;

    bool battery;
    bool battery_enabled = false;

    void battery_load(){
        char fn[1048];
        sprintf(fn, "%s.battery", filename);
        FILE *fp = fopen(fn, "rb");
        printf("trying to load\n");

        if(fp){
            printf("loaded\n");
            fread(ram_banks, ram_size, 1, fp);
            fclose(fp);
        }
    }

    void battery_save(){
        char fn[1048];
        sprintf(fn, "%s.battery", filename);
        FILE *fp = fopen(fn, "wb");

        if(fp){
            printf("saved\n");
            fwrite(ram_banks, ram_size, 1, fp);
            fclose(fp);
        }
    }

    void load_rom(char *cart){
        snprintf(filename, sizeof(filename), "%s", cart);

        FILE *fp = fopen(cart, "r");

        if (!fp) {
            printf("Failed to open: %s\n", cart);
            exit(-2);
        }

        fseek(fp, 0, SEEK_END);
        rom_size = ftell(fp);

        rewind(fp);
        printf("rom size: %08X",rom_size);

        rom_data = (uint8_t *)(malloc(rom_size));
        fread(rom_data, rom_size, 1, fp);
        fclose(fp);

        printf("Cartridge Loaded:\n");

        cart_type = rom_data[0x0147];
        cur_rom_bank = rom_data + 0x4000;

        if(rom_data[0x0149] > 0x1){
            switch(rom_data[0x0149]){
                case 0x02:
                    ram_banks = (uint8_t *)(malloc(sizeof(uint8_t) * 8000)); ram_size = 0x2000; break;
                case 0x03:
                    ram_banks = (uint8_t *)(malloc(sizeof(uint8_t) * 8000 * 4)); ram_size = 0x7D00; break;
                case 0x04:
                    ram_banks = (uint8_t *)(malloc(sizeof(uint8_t) * 8000 * 16)); ram_size = 0xFA00; break;
                case 0x05:
                    ram_banks = (uint8_t *)(malloc(sizeof(uint8_t) * 8000 * 8)); ram_size = 0x1F400; break;
            }
            cur_ram_bank = ram_banks;
            ram_enabled = true;
        }
        else ram_size = 0;

        if(cart_type == 0x10 || cart_type == 0x13 || cart_type == 0x3) {
            battery = true;
            //battery_load();
        }
        else battery = false;


    };

    uint8_t read(uint16_t address);
    void write(uint16_t address, uint8_t value);

    uint8_t rom_bank_read(uint16_t address);
    void rom_bank_write(uint16_t address, uint8_t value);

    uint8_t ram_read(uint16_t address);
    void ram_write(uint16_t address, uint8_t value);
};
