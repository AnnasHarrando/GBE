#include <cstdint>
#include "inst.h"
#include "emu.h"
#include <map>

#define BIT_SET(n, on) { if (on) regists.f = regists.f | (1 << n); else regists.f = regists.f & ~(1 << n);}

typedef struct{
    union {
        struct {
            uint8_t f;
            uint8_t a;
        };
        uint16_t af;
    };

    union {
        struct {
            uint8_t c;
            uint8_t b;
        };
        uint16_t bc;
    };

    union {
        struct {
            uint8_t e;
            uint8_t d;
        };
        uint16_t de;
    };

    union {
        struct {
            uint8_t l;
            uint8_t h;
        };
        uint16_t hl;
    };

    uint16_t sp;
}registers;



class cpu{
public:
    registers regists;

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
        regists.af = 0x01B0;
        regists.bc = 0x0013;
        regists.de = 0x00D8;
        regists.hl = 0x014D;
        regists.sp = 0xFFFE;

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
    uint8_t get_ie_register();
    void set_ie_register(uint8_t val);
    void set_register(REG regis,uint16_t val);
    uint16_t get_register(REG regis);
    void check_interrupt();
    bool check_int(uint16_t addr, uint8_t interrupt);
    void get_interrupt(uint8_t interrupt);
    void cb();
    void it_push(uint16_t val);
};






