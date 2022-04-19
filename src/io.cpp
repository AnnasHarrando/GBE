//
// Created by annas on 13/04/2022.
//

#include "io.h"

uint8_t io::io_read(uint16_t addr) {
    if(addr == 0xFF01) return serial_data[0];
    if(addr == 0xFF02) return serial_data[1];

    return 0;
}

void io::io_write(uint16_t addr, uint8_t val) {
    if(addr == 0xFF01) serial_data[0] = val;
    if(addr == 0xFF02) serial_data[1] = val;
}
