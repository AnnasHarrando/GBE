#include "cpu.h"
#include "cart.h"
#include "inst.h"
#include "bus.h"
#include <iostream>
#include <cstdlib>

using namespace std;

context cpu = { };

void cpu_init(){
    cpu.regs = {{A,1}, {B,0}, {C,0}, {D,0}, {E,0}, {F,0}, {H,0}, {L,0}, {AF,1<<8}, {BC,0}, {DE,0}, {HL,0}, {SP,0}};
     cpu.pc = 0x100;
}


void step(){

    uint16_t cur_pc =  cpu.pc;
    cpu.saved_regs = cpu.regs;

    cpu.opcode = bus_read( cpu.pc++);
    cpu.cur_inst = get_instruction(cpu.opcode);
    printf("%04X: %-7s (%02X %02X %02X)\n", cur_pc, inst_name(cpu.cur_inst->type), cpu.opcode, bus_read( cpu.pc), bus_read( cpu.pc+1));
    printf("AF: %02X%02X, BC: %02X%02X, DE: %02X%02X, HL: %02X%02X\n\n",cpu.regs[A],cpu.regs[F],cpu.regs[B],cpu.regs[C],cpu.regs[D],cpu.regs[E],cpu.regs[H],cpu.regs[L]);

    cpu.load_in_mem = 0;
    fetch_data();
    exec();
    correct_regs();
    if( cpu.pc == 500) exit(-2);
}

void fetch_data(){
    switch(cpu.cur_inst->mode){
        case IMP:
            break;
        case R:
            cpu.fetch = cpu.regs[cpu.cur_inst->reg_1];
            break;
        case R_R:
            cpu.fetch = cpu.regs[cpu.cur_inst->reg_2];
            break;
        case R_D8:
        case D8:
            cpu.fetch = bus_read( cpu.pc);
             cpu.pc++;
            break;
        case R_D16:
        case D16: {
            uint16_t low = bus_read( cpu.pc);
             cpu.pc++;
            uint16_t high = bus_read( cpu.pc);
             cpu.pc++;
            cpu.fetch = low | (high << 8);
        }
            break;
        case D16_R:
        case A16_R: {
            uint16_t low = bus_read( cpu.pc);
             cpu.pc++;
            uint16_t high = bus_read( cpu.pc);
             cpu.pc++;
            cpu.load_in_mem = low | (high << 8);
            cpu.fetch = cpu.regs[cpu.cur_inst->reg_1];
            }
            break;
        case R_A8:
            cpu.fetch = bus_read( cpu.pc);
             cpu.pc++;
            break;
        case A8_R:
            cpu.load_in_mem = bus_read( cpu.pc) | 0xFF00;
             cpu.pc++;
            cpu.fetch = cpu.regs[cpu.cur_inst->reg_1];
            break;
        case R_A16:{
            uint16_t low = bus_read( cpu.pc);
             cpu.pc++;
            uint16_t high = bus_read( cpu.pc);
             cpu.pc++;
            cpu.fetch = bus_read(low | (high << 8));
        } break;
        case MR:
            cpu.load_in_mem = cpu.regs[cpu.cur_inst->reg_1];
            break;
        case R_MR:
            if(cpu.cur_inst->reg_2 == C) cpu.fetch = cpu.regs[cpu.cur_inst->reg_2] |= 0xFF00;
            else cpu.fetch = cpu.regs[cpu.cur_inst->reg_2];
            break;
        case MR_R:
            cpu.load_in_mem = cpu.regs[cpu.cur_inst->reg_1];
            if(cpu.cur_inst->reg_1 == C) cpu.load_in_mem |= 0xFF00;
            cpu.fetch = cpu.regs[cpu.cur_inst->reg_2];
            break;
        case MR_D8:
            cpu.load_in_mem = cpu.regs[cpu.cur_inst->reg_1];
            cpu.fetch = bus_read( cpu.pc);
             cpu.pc++;
            break;
        case HLD_R:
            cpu.load_in_mem = cpu.regs[HL];
            cpu.fetch = cpu.regs[A];
            cpu.regs[HL]--;
            break;
        case HLI_R:
            cpu.load_in_mem = cpu.regs[HL];
            cpu.fetch = cpu.regs[A];
            cpu.regs[HL]++;
            break;
        case HL_SPR:{
            signed char temp = (signed char) bus_read( cpu.pc);
            cpu.regs[SP] += temp;
            cpu.fetch = cpu.regs[SP];
        }
        break;
        default:
            printf("Unknown Addressing Mode! %d (%02X)\n", cpu.cur_inst->mode, cpu.opcode);
            exit(-7);
    }
}



bool cond(){
    bool z = ((cpu.regs[F])>>(7)) & 1;
    bool c = ((cpu.regs[F])>>(4)) & 1;

    switch(cpu.cur_inst->cond) {
        case  CT_NONE: return true;
        case  CT_C: return c;
        case  CT_NC: return !c;
        case  CT_Z: return z;
        case  CT_NZ: return !z;
    }
}

void exec(){
    switch(cpu.cur_inst->type){
        case I_NONE:
            printf("Unknown type   I_NONE");
            exit(-3);
            break;
        case NOP:
            break;
        case JP:
            if(cond())  cpu.pc = cpu.fetch;
            break;
        case XOR:
            cpu.regs[A] ^= cpu.fetch;
            set_flags(cpu.regs[A],0,0,0);
            break;
        case LD:
            if(cpu.load_in_mem != 0){
                if(cpu.opcode == 0x08){
                    bus_write(cpu.load_in_mem,cpu.regs[SP] & 0xFF);
                    bus_write(cpu.load_in_mem+1,cpu.regs[SP] >> 8);
                }
                printf("%04X\n",cpu.load_in_mem);
                bus_write(cpu.load_in_mem,cpu.regs[cpu.cur_inst->reg_1]);
            }
            else cpu.regs[cpu.cur_inst->reg_1] = cpu.fetch;
            if(cpu.opcode == 0xF8) set_flags(0,0,(cpu.regs[cpu.cur_inst->reg_2] & 0xF) + (cpu.fetch & 0xF) > 0xF,(cpu.regs[cpu.cur_inst->reg_2] & 0xFF) + (cpu.fetch & 0xFF) > 0xFF);
            break;
        case DEC:
            if(cpu.load_in_mem != 0) {
                bus_write(cpu.load_in_mem, bus_read(cpu.load_in_mem)-1);
                set_flags(bus_read(cpu.load_in_mem) == 0,1,(bus_read(cpu.load_in_mem) & 0xF) == 0xF,-1);
            }
            else {
                cpu.regs[cpu.cur_inst->reg_1]--;
                if ((cpu.opcode & 0xB) != 0xB) set_flags(cpu.regs[cpu.cur_inst->reg_1] == 0,1,(cpu.regs[cpu.cur_inst->reg_1] & 0xF) == 0xF,-1);
            }
            break;
        case INC:
            if(cpu.opcode == 0x34) {
                bus_write(cpu.load_in_mem, bus_read(cpu.load_in_mem)+1);
                set_flags(bus_read(cpu.load_in_mem) == 0,0,(bus_read(cpu.load_in_mem) & 0xF) == 0xF,-1);
            }
            else {
                cpu.regs[cpu.cur_inst->reg_1]++;
                if ((cpu.opcode & 0x03) != 0x03) set_flags(cpu.regs[cpu.cur_inst->reg_1] == 0,0,(cpu.regs[cpu.cur_inst->reg_1] & 0xF) == 0xF,-1);
            }
            break;
        case JR:
            if(cond()){
                 cpu.pc += (signed char)(cpu.fetch & 0xFF);
            }
            break;
        case RRCA: {
            uint8_t temp = cpu.regs[A];
            cpu.regs[A] = (cpu.regs[A] >> 1) || ((cpu.regs[A] & 0x1) << 7);
            set_flags(0, 0, 0, temp);
        }
            break;
        case RRA: {
            uint8_t temp = cpu.regs[A] & 0x1;
            cpu.regs[A] = (cpu.regs[A] >> 1) || (cpu.regs[F] & 0x80);
            set_flags(0, 0, 0, temp);
        }
            break;
        case OR:
            if(cpu.load_in_mem != 0) cpu.regs[A] |= bus_read(cpu.load_in_mem);
            else cpu.regs[A] |= cpu.fetch;
            set_flags(cpu.regs[A],0,0,0);
            break;
        case CP: {
            int temp = (int) cpu.regs[A] - (int) cpu.fetch;
            set_flags(temp == 0, 1, ((int) cpu.regs[A] & 0xF) - ((int) cpu.fetch & 0xF) < 0, temp < 0);
        }
            break;
        case ADD:
            switch (cpu.opcode & 0xF0) {
                case 0x80:
                case 0xC0:
                    cpu.regs[cpu.cur_inst->reg_1] += cpu.fetch;
                    set_flags(cpu.regs[cpu.cur_inst->reg_1] == 0,0,(cpu.regs[cpu.cur_inst->reg_1] & 0xF) + (cpu.fetch & 0xF) > 0x0F,(int)cpu.regs[cpu.cur_inst->reg_1] + (int)cpu.fetch > 0xFF);
                    break;
                case 0xE0: {
                    signed char temp = (signed char) cpu.fetch;
                    cpu.regs[SP] += temp;
                    set_flags(0, 0, (cpu.regs[cpu.cur_inst->reg_1] & 0xFF) + (cpu.fetch & 0xFF) > 0xFF,
                              (uint32_t) cpu.regs[cpu.cur_inst->reg_1] + (uint32_t) cpu.fetch > 0xFFFF);
                }
                    break;
                default:
                    cpu.regs[cpu.cur_inst->reg_1] += cpu.fetch;
                    set_flags(-1, 0, (cpu.regs[cpu.cur_inst->reg_1] & 0xFF) + (cpu.fetch & 0xFF) > 0xFF,
                              (uint32_t) cpu.regs[cpu.cur_inst->reg_1] + (uint32_t) cpu.fetch > 0xFFFF);
            }
            break;
        case ADC:{
            uint16_t a = cpu.regs[A];
            cpu.regs[A] = (cpu.regs[A] + cpu.fetch + (cpu.regs[F] >> 4) & 0x1) & 0xFF;
            set_flags(a == 0, 0, (a & 0xF) + (cpu.fetch & 0xF) + ((cpu.regs[F] >> 4) & 0x1) > 0xF, a+cpu.fetch+((cpu.regs[F] >> 4) & 0x1) > 0xFF);
            }
            break;
        case SUB:
            set_flags(cpu.regs[A] - cpu.fetch == 0, 1, (int)(cpu.regs[A] & 0xF) - (int)(cpu.fetch & 0xF) < 0,
                      (int)cpu.regs[A] - (int)cpu.fetch < 0);
            cpu.regs[A] -= cpu.fetch;
            break;
        case SBC:
            set_flags(cpu.regs[A] - cpu.fetch - ((cpu.regs[F] >> 4) & 0x1) == 0, 1,
                      (int)(cpu.regs[A] & 0xF) - (int)(cpu.fetch & 0xF) - (int)((cpu.regs[F] >> 4) & 0x1) < 0,
                      (int)cpu.regs[A] - (int)cpu.fetch - (int)((cpu.regs[F] >> 4) & 0x1) < 0);
            cpu.regs[A] -= (cpu.fetch + (cpu.regs[F] >> 4) & 0x1);
            break;
        case RLCA: {
            uint8_t temp = cpu.regs[A] >> 7;
            cpu.regs[A] = (cpu.regs[A] << 1) || temp;
            set_flags(0,0,0,temp);
            }
            break;
        case RLA: {
            uint8_t temp = cpu.regs[A] >> 7;
            cpu.regs[A] = (cpu.regs[A] << 1) || (cpu.regs[F] & 0x80) >> 7;
            set_flags(0, 0, 0, temp);
            }
            break;
        case DI:
            cpu.master_enabled = false;
            break;
        case POP:
            if(cpu.cur_inst->reg_1 == AF) cpu.regs[AF] = pop() & 0xFFF0;
            else cpu.regs[cpu.cur_inst->reg_1] = pop();
            break;
        case PUSH:
            push(cpu.regs[cpu.cur_inst->reg_1]);
            break;
        case CALL:
            if(cond()){
                push(cpu.pc);
                cpu.pc = cpu.fetch;
            }
            break;
        case RET:
            if(cond()) cpu.pc = pop();
            break;
        case RETI:
            cpu.master_enabled = true;
            cpu.pc = pop();
            break;
        case RST:
            push(cpu.pc);
            cpu.pc = bus_read(cpu.cur_inst->param);
            break;
        default:
            printf("Unknown type");
            exit(-3);
            break;
    }
}

void set_flags(uint8_t z, uint8_t n, uint8_t h, uint8_t c){
    if(z >= 0) BIT_SET(7,z);
    if(n >= 0) BIT_SET(6,n);
    if(h >= 0) BIT_SET(5,h);
    if(c >= 0) BIT_SET(4,c);
}

void correct_regs(){
    if(cpu.regs[A] != cpu.saved_regs[A] || cpu.regs[F] != cpu.saved_regs[F]) cpu.regs[AF] = cpu.regs[A] << 8 | cpu.regs[F];
    if(cpu.regs[B] != cpu.saved_regs[B] || cpu.regs[C] != cpu.saved_regs[C]) cpu.regs[BC] = cpu.regs[B] << 8 | cpu.regs[C];
    if(cpu.regs[D] != cpu.saved_regs[D] || cpu.regs[E] != cpu.saved_regs[E]) cpu.regs[DE] = cpu.regs[D] << 8 | cpu.regs[E];
    if(cpu.regs[H] != cpu.saved_regs[H] || cpu.regs[L] != cpu.saved_regs[L]) cpu.regs[HL] = cpu.regs[H] << 8 | cpu.regs[L];

    if(cpu.regs[BC] == cpu.saved_regs[BC]){
        cpu.regs[B] = cpu.regs[BC] >> 8;
        cpu.regs[C] = cpu.regs[BC] & 0x00FF;
    }
    else if(cpu.regs[DE] == cpu.saved_regs[DE]){
        cpu.regs[D] = cpu.regs[DE] >> 8;
        cpu.regs[E] = cpu.regs[DE] & 0x00FF;
    }
    else if(cpu.regs[HL] == cpu.saved_regs[HL]){
        cpu.regs[H] = cpu.regs[HL] >> 8;
        cpu.regs[L] = cpu.regs[HL] & 0x00FF;
    }
}

uint8_t get_ie_register(){
    return cpu.ie_register;
}
void set_ie_register(uint8_t val){
    cpu.ie_register = val;
};

void push(uint16_t val){
    cpu.regs[SP]--;
    bus_write(cpu.regs[SP],cpu.regs[BC] >> 8);
    cpu.regs[SP]--;
    bus_write(cpu.regs[SP],cpu.regs[BC] & 0xFF);
}

uint16_t pop(){
    uint8_t high = bus_read(cpu.regs[SP]++);
    uint8_t low = bus_read(cpu.regs[SP]++);

    return (high << 8) | low;
}

