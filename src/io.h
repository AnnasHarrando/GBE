#include <cstdint>

class io {
public:
    uint8_t serial_data[2];

    uint8_t io_read(uint16_t addr);
    void io_write(uint16_t addr, uint8_t val);

};
