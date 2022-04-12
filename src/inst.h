#pragma once
#include <cstdint>

typedef enum {
    IMP,
    R_D16,
    R_R,
    MR_R,
    R,
    R_D8,
    R_MR,
    R_HLI,
    R_HLD,
    HLI_R,
    HLD_R,
    R_A8,
    A8_R,
    HL_SPR,
    D16,
    D8,
    D16_R,
    MR_D8,
    MR,
    A16_R,
    R_A16
} ADDR;

typedef enum {
    R_NONE,
    A,
    F,
    B,
    C,
    D,
    E,
    H,
    L,
    AF,
    BC,
    DE,
    HL,
    SP
} REG;

typedef enum {
    I_NONE,
    NOP,
    LD,
    INC,
    DEC,
    RLCA,
    ADD,
    RRCA,
    STOP,
    RLA,
    JR,
    RRA,
    DAA,
    CPL,
    SCF,
    CCF,
    HALT,
    ADC,
    SUB,
    SBC,
    AND,
    XOR,
    OR,
    CP,
    POP,
    JP,
    PUSH,
    RET,
    CB,
    CALL,
    RETI,
    LDH,
    JPHL,
    DI,
    EI,
    RST,
    ERR,

    RLC,
    RRC,
    RL,
    RR,
    SLA,
    SRA,
    SWAP,
    SRL,
    BIT,
    RES,
    SET
} INST;

typedef enum {
     CT_NONE,  CT_NZ,  CT_Z,  CT_NC,  CT_C
} COND;

typedef struct instruction{

    INST type = I_NONE;
    ADDR mode = IMP;
    REG reg_1 = R_NONE;
    REG reg_2 = R_NONE;
    COND cond = CT_NONE;
    uint8_t param = 0x00;

} instruction;

instruction test();

void init();

struct instruction *get_instruction(uint8_t opcode);

char *inst_name(INST t);