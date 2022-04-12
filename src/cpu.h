#include <cstdint>
#include "inst.h"
#include <map>

#define BIT_SET(n, on) { if (on) cpu.regs[F] |= (1 << n); else cpu.regs[F] &= ~(1 << n);}


typedef struct {
    std::map<REG, uint16_t> regs;
    std::map<REG, uint16_t> saved_regs;
    uint8_t opcode;
    instruction *cur_inst;
    uint8_t ie_register;
    uint16_t fetch;
    uint16_t load_in_mem;
    uint16_t pc;
    bool master_enabled;
}context;

    void cpu_init();
    void step();
    void fetch_data();
    void exec();
    bool cond();
    void push(uint16_t val);
    uint16_t pop();
    void set_flags(uint8_t z, uint8_t n, uint8_t h, uint8_t c);

    void correct_regs();

    uint8_t get_ie_register();

    void set_ie_register(uint8_t val);





