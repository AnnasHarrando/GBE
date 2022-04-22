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
        regs = {{A,0x01}, {B,0x00}, {C,0x13}, {D,0x00}, {E,0xD8}, {F,0xB0}, {H,0x01}, {L,0x4D}, {AF,0x01B0}, {BC,0x0013}, {DE,0x00D8}, {HL,0x014D}, {SP,0xFFFE}};
        pc = 0x100;
        halt = false;
        ie_register = 0;
        int_flags = 0;
        master_enabled = false;
        ime = false;
    }

    void step();
    void fetch_data();
    void exec();
    bool cond();
    void push(uint16_t val);
    uint16_t pop();
    void set_flags(int z, int n, int h, int c);

    void correct_regs();

    uint8_t get_ie_register();

    void set_ie_register(uint8_t val);

    void check_interrupt();

    bool check_int(uint16_t addr, uint8_t interrupt);

    void get_interrupt(uint8_t interrupt);

    void cb();

    void it_push(uint16_t val);
};






