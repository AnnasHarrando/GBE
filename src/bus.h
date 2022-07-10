#ifndef GBE_BUS_H
#define GBE_BUS_H

#include <cstdint>


uint8_t bus_read(uint16_t addr, bool IsCPU = false);
void bus_write(uint16_t addr, uint8_t val);

void bus_init();


#endif //GBE_BUS_H
