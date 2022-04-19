#include <cstdint>
#include "inst.h"
#include "emu.h"
#include <map>

#define BIT_SET(n, on) { if (on) regs[F] |= (1 << n); else regs[F] &= ~(1 << n);}

class cpu{
public:
    std::map<REG, uint16_t> regs;
    std::map<REG, uint16_t> saved_regs;
    REG cb_reg;
    uint8_t cb_fetch;
    uint8_t opcode;
    instruction *cur_inst;
    uint8_t ie_register;
    uint8_t int_flags;
    uint16_t fetch;
    uint16_t load_in_mem;
    uint16_t pc;
    bool master_enabled;
    bool halt;
    bool ime;


    cpu(){
        regs = {{A,1}, {B,0}, {C,0}, {D,0}, {E,0}, {F,0}, {H,0}, {L,0}, {AF,1<<8}, {BC,0}, {DE,0}, {HL,0}, {SP,0xFFFE}};
        pc = 0x100;
        halt = false;
    }

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

    void check_interrupt();

    bool check_int(uint16_t addr, uint8_t interrupt);

    void cb();
};






