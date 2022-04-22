//
// Created by annas on 13/04/2022.
//
#include "io.h"
uint8_t ly = 0;
uint8_t io::io_read(uint16_t addr) {
    if(addr == 0xFF01) return serial_data[0];
    if(addr == 0xFF02) return serial_data[1];
    if((addr >= 0xFF04) && (addr <= 0xFF07)) return timer_read(addr);
    if(addr == 0xFF0F) return get_int_flags();
    if(addr == 0xFF44) {
        return ly++;
    }

    return 0;
}

void io::io_write(uint16_t addr, uint8_t val) {
    if(addr == 0xFF01) serial_data[0] = val;
    if(addr == 0xFF02) serial_data[1] = val;
    if((addr >= 0xFF04) && (addr <= 0xFF07)) timer_write(addr,val);
    if(addr == 0xFF0F) set_int_flags(val);
    if(addr == 0xFF46) {
        dma_start(val);
        printf("DMA START! val:%02X\n",val);
    }

}
