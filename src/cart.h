#include <cstdint>


bool cart_load(char *cart);

uint8_t cart_read(uint16_t address);
void cart_write(uint16_t address, uint8_t value);