#include <cstdint>
#include "inst.h"
#include <map>

#define BIT_SET(n, on) { if (on) regs[F] |= (1 << n); else regs[F] &= ~(1 << n);}

class cpu {
public:

    cpu(){
        init();
    }

    std::map<REG, uint16_t> regs {{A,1}, {B,0}, {C,0}, {D,0}, {E,0}, {F,0}, {H,0}, {L,0}, {AF,1<<8}, {BC,0}, {DE,0}, {HL,0}, {SP,0}};
    std::map<REG, uint16_t> saved_regs = regs;

    uint8_t opcode;
    instruction *cur_inst;
    uint16_t fetch;
    uint16_t load_in_mem;

    uint16_t pc = 0x100;

    void step();
    void fetch_data();
    void exec();
    bool cond();
    void set_flags(uint8_t z, uint8_t n, uint8_t h, uint8_t c);

    void correct_regs();
};



