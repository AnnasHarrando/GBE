//
// Created by annas on 13/04/2022.
//
#include "io.h"
#include "screen.h"
uint8_t io::read(uint16_t addr) {
    if(addr == 0xFF00) return get_button_press();
    if(addr == 0xFF01) return serial_data[0];
    if(addr == 0xFF02) return serial_data[1];
    if((addr >= 0xFF04) && (addr <= 0xFF07)) return timer_read(addr);
    if(addr == 0xFF0F) return get_int_flags();
    if((addr >= 0xFF40) && (addr <= 0xFF4B)) return lcd_read(addr);

    return 0;
}

void io::write(uint16_t addr, uint8_t val) {
    if(addr == 0xFF00) set_button_type(val);
    if(addr == 0xFF01) serial_data[0] = val;
    if(addr == 0xFF02) serial_data[1] = val;
    if((addr >= 0xFF04) && (addr <= 0xFF07)) timer_write(addr,val);
    if(addr == 0xFF0F) set_int_flags(val);
    if((addr >= 0xFF40) && (addr <= 0xFF4B)) lcd_write(addr,val);
    if(addr == 0xFF46) {
        dma_start(val);
    }

}
