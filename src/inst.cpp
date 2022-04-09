#include "inst.h"
instruction instructions[0x100];

void init(){

    instructions[0x00] =  {NOP, IMP};

    instructions[0x05] =  {DEC, R, B};
    instructions[0x06] =  {LD, R_D8, B};
    instructions[0x07] =  {RLCA};
    instructions[0x08] =  {LD, A16_R, R_NONE, SP};
    instructions[0x09] =  {ADD, R_R, HL, BC};

    instructions[0x0B] =  {DEC, R, BC};

    instructions[0x0E] =  {LD, R_D8, C};
    instructions[0x0F] =  {RRCA};

    instructions[0x14] =  {INC, R, D};
    instructions[0x15] =  {DEC, R, D};
    instructions[0x16] =  {LD,R_D8, D};
    instructions[0x17] =  {RLA};

    instructions[0x18] =  {JR, D8};
    instructions[0x19] =  {ADD, R_R, HL, DE};

    instructions[0x1B] =  {DEC, R, DE};

    instructions[0x1F] =  {RRA};

    instructions[0x20] =  {JR, D8, R_NONE, R_NONE,  CT_NZ};
    instructions[0x21] =  {LD, R_D16, HL};
    instructions[0x22] =  {LD, HLI_R, HL, A};

    instructions[0x25] =  {DEC, R, H};
    instructions[0x26] =  {LD,R_D8, H};

    instructions[0x28] =  {JR, D8, R_NONE, R_NONE,  CT_Z};
    instructions[0x29] =  {ADD, R_R, HL, HL};

    instructions[0x2B] =  {DEC, R, HL};


    instructions[0x30] =  {JR, D8, R_NONE, R_NONE,CT_NC};
    instructions[0x31] =  {DEC, R, DE};
    instructions[0x32] =  {LD, HLD_R, HL, A};

    instructions[0x38] =  {JR, D8, R_NONE, R_NONE, CT_C};
    instructions[0x39] =  {ADD, R_R, HL, SP};

    instructions[0x3B] =  {DEC, R, SP};

    instructions[0xAF] =  {XOR, R, A};

    instructions[0xC3] =  {JP, D16};

    instructions[0xF3] =  {DI};

    instructions[0x40] =  {LD, R_R, B, B};
    instructions[0x41] =  {LD, R_R, B, C};
    instructions[0x42] =  {LD, R_R, B, D};
    instructions[0x43] =  {LD, R_R, B, E};
    instructions[0x44] =  {LD, R_R, B, H};
    instructions[0x45] =  {LD, R_R, B, L};
    instructions[0x46] =  {LD, R_MR, B, HL};
    instructions[0x47] =  {LD, R_R, B, A};
    instructions[0x48] =  {LD, R_R, C, B};
    instructions[0x49] =  {LD, R_R, C, C};
    instructions[0x4A] =  {LD, R_R, C, D};
    instructions[0x4B] =  {LD, R_R, C, E};
    instructions[0x4C] =  {LD, R_R, C, H};
    instructions[0x4D] =  {LD, R_R, C, L};
    instructions[0x4E] =  {LD, R_MR, C, HL};
    instructions[0x4F] =  {LD, R_R, C, A};

    //0x5X
    instructions[0x50] =  {LD, R_R,  D, B};
    instructions[0x51] =  {LD, R_R,  D, C};
    instructions[0x52] =  {LD, R_R,  D, D};
    instructions[0x53] =  {LD, R_R,  D, E};
    instructions[0x54] =  {LD, R_R,  D, H};
    instructions[0x55] =  {LD, R_R,  D, L};
    instructions[0x56] =  {LD, R_MR, D, HL};
    instructions[0x57] =  {LD, R_R,  D, A};
    instructions[0x58] =  {LD, R_R,  E, B};
    instructions[0x59] =  {LD, R_R,  E, C};
    instructions[0x5A] =  {LD, R_R,  E, D};
    instructions[0x5B] =  {LD, R_R,  E, E};
    instructions[0x5C] =  {LD, R_R,  E, H};
    instructions[0x5D] =  {LD, R_R,  E, L};
    instructions[0x5E] =  {LD, R_MR, E, HL};
    instructions[0x5F] =  {LD, R_R,  E, A};

    //0x6X
    instructions[0x60] =  {LD, R_R,  H, B};
    instructions[0x61] =  {LD, R_R,  H, C};
    instructions[0x62] =  {LD, R_R,  H, D};
    instructions[0x63] =  {LD, R_R,  H, E};
    instructions[0x64] =  {LD, R_R,  H, H};
    instructions[0x65] =  {LD, R_R,  H, L};
    instructions[0x66] =  {LD, R_MR, H, HL};
    instructions[0x67] =  {LD, R_R,  H, A};
    instructions[0x68] =  {LD, R_R,  L, B};
    instructions[0x69] =  {LD, R_R,  L, C};
    instructions[0x6A] =  {LD, R_R,  L, D};
    instructions[0x6B] =  {LD, R_R,  L, E};
    instructions[0x6C] =  {LD, R_R,  L, H};
    instructions[0x6D] =  {LD, R_R,  L, L};
    instructions[0x6E] =  {LD, R_MR, L, HL};
    instructions[0x6F] =  {LD, R_R,  L, A};

    //0x7X
    instructions[0x70] =  {LD, MR_R,  HL, B};
    instructions[0x71] =  {LD, MR_R,  HL, C};
    instructions[0x72] =  {LD, MR_R,  HL, D};
    instructions[0x73] =  {LD, MR_R,  HL, E};
    instructions[0x74] =  {LD, MR_R,  HL, H};
    instructions[0x75] =  {LD, MR_R,  HL, L};
    //instructions[0x76] =  {HALT};
    instructions[0x77] =  {LD, MR_R,  HL, A};
    instructions[0x78] =  {LD, R_R,  A, B};
    instructions[0x79] =  {LD, R_R,  A, C};
    instructions[0x7A] =  {LD, R_R,  A, D};
    instructions[0x7B] =  {LD, R_R,  A, E};
    instructions[0x7C] =  {LD, R_R,  A, H};
    instructions[0x7D] =  {LD, R_R,  A, L};
    instructions[0x7E] =  {LD, R_MR, A, HL};
    instructions[0x7F] =  {LD, R_R,  A, A};

    instructions[0x80] =  {ADD, R_R, A, B};
    instructions[0x81] =  {ADD, R_R, A, C};
    instructions[0x82] =  {ADD, R_R, A, D};
    instructions[0x83] =  {ADD, R_R, A, E};
    instructions[0x84] =  {ADD, R_R, A, H};
    instructions[0x85] =  {ADD, R_R, A, L};
    instructions[0x86] =  {ADD, R_MR, A, HL};
    instructions[0x87] =  {ADD, R_R, A, A};

    instructions[0xB0] =  {OR, R, B};
    instructions[0xB1] =  {OR, R, C};
    instructions[0xB2] =  {OR, R, D};
    instructions[0xB3] =  {OR, R, E};
    instructions[0xB4] =  {OR, R, H};
    instructions[0xB5] =  {OR, R, L};
    instructions[0xB6] =  {OR, MR, HL};
    instructions[0xB7] =  {OR, R, A};
    instructions[0xB8] =  {CP, R, B};
    instructions[0xB9] =  {CP, R, C};
    instructions[0xBA] =  {CP, R, D};
    instructions[0xBB] =  {CP, R, E};
    instructions[0xBC] =  {CP, R, H};
    instructions[0xBD] =  {CP, R, L};
    instructions[0xBE] =  {CP, MR, HL};
    instructions[0xBF] =  {CP, R, A};

    instructions[0xC6] =  {ADD, R_D8, A};

    instructions[0xE8] =  {ADD, R_D8, SP};

    instructions[0xF6] =  {OR, D8};
    instructions[0xFE] =  {CP, D8};

}

instruction test(){
    return instructions[0];
}

instruction *get_instruction(uint8_t opcode) {
    return &instructions[opcode];
}

char *inst_lookup[] = {
        "<NONE>",
        "NOP",
        "LD",
        "INC",
        "DEC",
        "RLCA",
        "ADD",
        "RRCA",
        "STOP",
        "RLA",
        "JR",
        "RRA",
        "DAA",
        "CPL",
        "SCF",
        "CCF",
        "HALT",
        "ADC",
        "SUB",
        "SBC",
        "AND",
        "XOR",
        "OR",
        "CP",
        "POP",
        "JP",
        "PUSH",
        "RET",
        "CB",
        "CALL",
        "RETI",
        "LDH",
        "JPHL",
        "DI",
        "EI",
        "RST",
        "IN_ERR",
        "IN_RLC",
        "IN_RRC",
        "IN_RL",
        "IN_RR",
        "IN_SLA",
        "IN_SRA",
        "IN_SWAP",
        "IN_SRL",
        "IN_BIT",
        "IN_RES",
        "IN_SET"
};

char *inst_name(INST t) {
    return inst_lookup[t];
}