#include <iostream>
#include "inst.h"

using namespace std;
static instruction instructions[0x100];

void init(){

    instructions[0x00] =  {NOP, IMP};
    instructions[0x01] =  {LD,R_D16,BC};
    instructions[0x02] =  {LD,D16_R,BC,A};
    instructions[0x03] =  {INC,R,BC};
    instructions[0x04] =  {INC,R,B};
    instructions[0x05] =  {DEC, R, B};
    instructions[0x06] =  {LD, R_D8, B};
    instructions[0x07] =  {RLCA};
    instructions[0x08] =  {LD, A16_R, SP};
    instructions[0x09] =  {ADD, R_R, HL, BC};
    instructions[0x0A] =  {LD,R_MR,A,BC};
    instructions[0x0B] =  {DEC, R, BC};
    instructions[0x0C] =  {INC, R, C};
    instructions[0x0D] =  {DEC, R, C};
    instructions[0x0E] =  {LD, R_D8, C};
    instructions[0x0F] =  {RRCA};

    instructions[0x10] =  {STOP};
    instructions[0x11] =  {LD,R_D16,DE};
    instructions[0x12] =  {LD,MR_R,DE,A};
    instructions[0x13] =  {INC,R,DE};
    instructions[0x14] =  {INC, R, D};
    instructions[0x15] =  {DEC, R, D};
    instructions[0x16] =  {LD,R_D8, D};
    instructions[0x17] =  {RLA};
    instructions[0x18] =  {JR, D8};
    instructions[0x19] =  {ADD, R_R, HL, DE};
    instructions[0x1A] =  {LD,R_MR,A,DE};
    instructions[0x1B] =  {DEC, R, DE};
    instructions[0x1C] =  {INC,R,E};
    instructions[0x1D] =  {DEC,R,E};
    instructions[0x1E] =  {LD,R_D8,E};
    instructions[0x1F] =  {RRA};

    instructions[0x20] =  {JR, D8, R_NONE, R_NONE,  CT_NZ};
    instructions[0x21] =  {LD, R_D16, HL};
    instructions[0x22] =  {LD, HLI_R, HL, A};
    instructions[0x23] =  {INC,R,HL};
    instructions[0x24] =  {INC,R,H};
    instructions[0x25] =  {DEC, R, H};
    instructions[0x26] =  {LD,R_D8, H};
    instructions[0x27] =  {DAA};
    instructions[0x28] =  {JR, D8, R_NONE, R_NONE,  CT_Z};
    instructions[0x29] =  {ADD, R_R, HL, HL};
    instructions[0x2A] =  {LD,R_HLI,A,HL};
    instructions[0x2B] =  {DEC, R, HL};
    instructions[0x2C] =  {INC,R,L};
    instructions[0x2D] =  {DEC,R,L};
    instructions[0x2E] =  {LD,R_D8,L};
    instructions[0x2F] =  {CPL};


    instructions[0x30] =  {JR, D8, R_NONE, R_NONE,CT_NC};
    instructions[0x31] =  {LD, R_D16, SP};
    instructions[0x32] =  {LD, HLD_R, HL, A};
    instructions[0x33] =  {INC,R,SP};
    instructions[0x34] =  {INC, MR, HL};
    instructions[0x35] =  {DEC, MR, HL};
    instructions[0x36] =  {LD,MR_D8,HL};
    instructions[0x37] =  {SCF};
    instructions[0x38] =  {JR, D8, R_NONE, R_NONE, CT_C};
    instructions[0x39] =  {ADD, R_R, HL, SP};
    instructions[0x3A] =  {LD,R_HLD,A,HL};
    instructions[0x3B] =  {DEC, R, SP};
    instructions[0x3C] =  {INC, R, A};
    instructions[0x3D] =  {DEC, R, A};
    instructions[0x3E] =  {LD,R_D8,A};
    instructions[0x3F] =  {CCF};

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
    instructions[0x76] =  {HALT};
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
    instructions[0x88] = {ADC, R_R, A, B};
    instructions[0x89] = {ADC, R_R, A, C};
    instructions[0x8A] = {ADC, R_R, A, D};
    instructions[0x8B] = {ADC, R_R, A, E};
    instructions[0x8C] = {ADC, R_R, A, H};
    instructions[0x8D] = {ADC, R_R, A, L};
    instructions[0x8E] = {ADC, R_MR, A, HL};
    instructions[0x8F] = {ADC, R_R, A, A};

    instructions[0x90] = {SUB, R,B};
    instructions[0x91] = {SUB, R,C};
    instructions[0x92] = {SUB, R, D};
    instructions[0x93] = {SUB, R,E};
    instructions[0x94] = {SUB, R,H};
    instructions[0x95] = {SUB, R,L};
    instructions[0x96] = {SUB, MR,HL};
    instructions[0x97] = {SUB, R,A};
    instructions[0x98] = {SBC, R,B};
    instructions[0x99] = {SBC, R,C};
    instructions[0x9A] = {SBC, R,D};
    instructions[0x9B] = {SBC, R,E};
    instructions[0x9C] = {SBC, R,H};
    instructions[0x9D] = {SBC, R,L};
    instructions[0x9E] = {SBC, MR,HL};
    instructions[0x9F] = {SBC, R,A};

    instructions[0xA0] = {AND, R, B};
    instructions[0xA1] = {AND, R, C};
    instructions[0xA2] = {AND, R, D};
    instructions[0xA3] = {AND, R, E};
    instructions[0xA4] = {AND, R, H};
    instructions[0xA5] = {AND, R, L};
    instructions[0xA6] = {AND, MR, HL};
    instructions[0xA7] = {AND, R, A};
    instructions[0xA8] = {XOR, R, B};
    instructions[0xA9] = {XOR, R, C};
    instructions[0xAA] = {XOR, R, D};
    instructions[0xAB] = {XOR, R, E};
    instructions[0xAC] = {XOR, R, H};
    instructions[0xAD] = {XOR, R, L};
    instructions[0xAE] = {XOR, MR, HL};
    instructions[0xAF] =  {XOR, R, A};

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

    instructions[0xC0] =  {RET,IMP,R_NONE,R_NONE,CT_NZ};
    instructions[0xC1] =  {POP,IMP,BC};
    instructions[0xC2] =  {JP,D16,R_NONE,R_NONE,CT_NZ};
    instructions[0xC3] =  {JP, D16};
    instructions[0xC4] =  {CALL,D16,R_NONE,R_NONE,CT_NZ};
    instructions[0xC5] =  {PUSH,IMP,BC};
    instructions[0xC6] =  {ADD, R_D8, A};
    instructions[0xC7] =  {RST, IMP, R_NONE, R_NONE, CT_NONE, 0x0000};
    instructions[0xC8] =  {RET,IMP,R_NONE,R_NONE,CT_Z};
    instructions[0xC9] =  {RET};
    instructions[0xCA] =  {JP,D16,R_NONE,R_NONE,CT_Z};
    instructions[0xCB] =  {CB,D8};
    instructions[0xCC] =  {CALL,D16,R_NONE,R_NONE,CT_Z};
    instructions[0xCD] =  {CALL,D16};
    instructions[0xCE] =  {ADC,R_D8,A};
    instructions[0xCF] =  {RST, IMP, R_NONE, R_NONE, CT_NONE, 0x0008};

    instructions[0xD0] =  {RET,IMP,R_NONE,R_NONE,CT_NC};
    instructions[0xD1] =  {POP,IMP,DE};
    instructions[0xD2] =  {JP,D16,R_NONE,R_NONE,CT_NC};

    instructions[0xD4] =  {CALL,D16,R_NONE,R_NONE,CT_NC};
    instructions[0xD5] =  {PUSH,IMP,DE};
    instructions[0xD6] =  {SUB,D8};
    instructions[0xD7] =  {RST, IMP, R_NONE, R_NONE, CT_NONE, 0x0010};
    instructions[0xD8] =  {RET,IMP,R_NONE,R_NONE,CT_C};
    instructions[0xD9] =  {RETI};
    instructions[0xDA] =  {JP,D16,R_NONE,R_NONE,CT_C};

    instructions[0xDC] =  {CALL,D16,R_NONE,R_NONE,CT_C};

    instructions[0xDE] =  {SBC,D8};
    instructions[0xDF] =  {RST, IMP, R_NONE, R_NONE, CT_NONE, 0x0018};

    instructions[0xE0] =  {LD,A8_R,A};
    instructions[0xE1] =  {POP,IMP,HL};
    instructions[0xE2] =  {LD,MR_R,C,A};

    instructions[0xE5] =  {PUSH,IMP,HL};
    instructions[0xE6] =  {AND,D8};
    instructions[0xE7] =  {RST, IMP, R_NONE, R_NONE, CT_NONE, 0x0020};
    instructions[0xE8] =  {ADD, R_D8, SP};
    instructions[0xE9] =  {JP,R,HL};
    instructions[0xEA] =  {LD,A16_R,A};

    instructions[0xEE] =  {XOR,D8};
    instructions[0xEF] =  {RST, IMP, R_NONE, R_NONE, CT_NONE, 0x0028};

    instructions[0xF0] =  {LD,R_A8,A};
    instructions[0xF1] =  {POP,IMP,AF};
    instructions[0xF2] =  {LD,R_MR,A,C};
    instructions[0xF3] =  {DI};

    instructions[0xF5] =  {PUSH,IMP,AF};
    instructions[0xF6] =  {OR, D8};
    instructions[0xF7] =  {RST, IMP, R_NONE, R_NONE, CT_NONE, 0x0030};
    instructions[0xF8] =  {LD, HL_SPR,HL};
    instructions[0xF9] =  {LD,R_R,SP,HL};
    instructions[0xFA] =  {LD,R_A16,A};
    instructions[0xFB] =  {EI};

    instructions[0xFE] =  {CP, D8};
    instructions[0xFF] =  {RST, IMP, R_NONE, R_NONE, CT_NONE, 0x0038};

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