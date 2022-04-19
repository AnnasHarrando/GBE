#include "cpu.h"
#include <iostream>
#include <cstdlib>

using namespace std;

void cpu::step(){
    if(!halt) {
        uint16_t cur_pc = pc;
        saved_regs = regs;

        opcode = bus_read(pc++);
        cur_inst = get_instruction(opcode);
        printf("%04X: %-7s (%02X %02X %02X)\n", cur_pc, inst_name(cur_inst->type), opcode, bus_read(pc),
               bus_read(pc + 1));
        printf("AF: %02X%02X, BC: %02X%02X, DE: %02X%02X, HL: %02X%02X\n", regs[A], regs[F], regs[B],
               regs[C], regs[D], regs[E], regs[H], regs[L]);
        printf("%04X %04X %04X %04X\n\n",regs[AF],regs[BC],regs[DE],regs[HL]);

        load_in_mem = 0;
        fetch_data();
        exec();
        correct_regs();
    }
    else{

        if(int_flags){
            halt = false;
        }
    }

    if(master_enabled) {
        check_interrupt();
        ime = true;
    }
    if(ime) master_enabled = true;

}

void cpu::fetch_data(){
    switch(cur_inst->mode){
        case IMP:
            break;
        case R:
            fetch = regs[cur_inst->reg_1];
            break;
        case R_R:
            fetch = regs[cur_inst->reg_2];
            break;
        case R_D8:
        case D8:
            fetch = bus_read( pc);
            pc++;
            break;
        case R_D16:
        case D16: {
            uint16_t low = bus_read( pc);
             pc++;
            uint16_t high = bus_read( pc);
             pc++;
            fetch = low | (high << 8);
        }
            break;
        case D16_R:
        case A16_R: {
            uint16_t low = bus_read( pc);
             pc++;
            uint16_t high = bus_read( pc);
             pc++;
            load_in_mem = low | (high << 8);
            fetch = regs[cur_inst->reg_1];
            }
            break;
        case R_A8:
            fetch = bus_read( pc);
             pc++;
            break;
        case A8_R:
            load_in_mem = bus_read( pc) | 0xFF00;
             pc++;
            fetch = regs[cur_inst->reg_1];
            break;
        case R_A16:{
            uint16_t low = bus_read( pc);
             pc++;
            uint16_t high = bus_read( pc);
             pc++;
            fetch = bus_read(low | (high << 8));
        } break;
        case MR:
            load_in_mem = regs[cur_inst->reg_1];
            break;
        case R_MR:
            if(cur_inst->reg_2 == C) fetch = bus_read(regs[cur_inst->reg_2] |= 0xFF00);
            else fetch = bus_read(regs[cur_inst->reg_2]);
            break;
        case MR_R:
            load_in_mem = regs[cur_inst->reg_1];
            printf("%04X ",load_in_mem);
            if(cur_inst->reg_1 == C) load_in_mem |= 0xFF00;
            fetch = regs[cur_inst->reg_2];
            printf("%04X\n",load_in_mem);
            break;
        case MR_D8:
            load_in_mem = regs[cur_inst->reg_1];
            fetch = bus_read( pc);
             pc++;
            break;
        case HLD_R:
            load_in_mem = regs[HL];
            fetch = regs[A];
            regs[HL]--;
            break;
        case HLI_R:
            load_in_mem = regs[HL];
            fetch = regs[A];
            regs[HL]++;
            break;
        case R_HLI:
            fetch = bus_read(regs[HL]);
            regs[HL]++;
            break;
        case R_HLD:
            fetch = bus_read(regs[HL]);
            regs[HL]--;
            break;
        case HL_SPR:{
            signed char temp = (signed char) bus_read( pc);
            regs[SP] += temp;
            fetch = regs[SP];
        }
        break;
        default:
            printf("Unknown Addressing Mode! %d (%02X)\n", cur_inst->mode, opcode);
            exit(-7);
    }
}



bool cpu::cond(){
    bool z = ((regs[F])>>(7)) & 1;
    bool c = ((regs[F])>>(4)) & 1;

    switch(cur_inst->cond) {
        case  CT_NONE: return true;
        case  CT_C: return c;
        case  CT_NC: return !c;
        case  CT_Z: return z;
        case  CT_NZ: return !z;
    }
}

void cpu::exec(){
    switch(cur_inst->type){
        case I_NONE:
            printf("Unknown type   I_NONE");
            exit(-3);
            break;
        case NOP:
            break;
        case JP:
            if(cond())  pc = fetch;
            break;
        case XOR:
            regs[A] ^= (fetch & 0xFF);
            set_flags(regs[A] == 0,0,0,0);
            break;
        case LD:
            if(load_in_mem != 0){
                if(opcode == 0x08){
                    bus_write(load_in_mem,regs[SP] & 0xFF);
                    bus_write(load_in_mem+1,regs[SP] >> 8);
                }
                printf("%04X\n",load_in_mem);
                bus_write(load_in_mem,fetch);
            }
            else regs[cur_inst->reg_1] = fetch;
            if(opcode == 0xF8) set_flags(0,0,(regs[cur_inst->reg_2] & 0xF) + (fetch & 0xF) > 0xF,(regs[cur_inst->reg_2] & 0xFF) + (fetch & 0xFF) > 0xFF);
            break;
        case DEC:
            if(opcode == 0x35) {
                bus_write(load_in_mem, bus_read(load_in_mem)-1);
                set_flags(bus_read(load_in_mem) == 0,1,(bus_read(load_in_mem) & 0xF) == 0xF,-1);
            }
            else {
                if(cur_inst->reg_1 < AF) regs[cur_inst->reg_1] = (regs[cur_inst->reg_1] - 1) & 0xFF;
                else regs[cur_inst->reg_1] = (regs[cur_inst->reg_1] - 1);
                if ((opcode & 0xB) != 0xB) set_flags(regs[cur_inst->reg_1] == 0,1,(regs[cur_inst->reg_1] & 0xF) == 0xF,-1);
            }
            break;
        case INC:
            if(opcode == 0x34) {
                bus_write(load_in_mem, bus_read(load_in_mem)+1);
                set_flags(bus_read(load_in_mem)-1 == 0,0,(bus_read(load_in_mem) & 0xF) == 0xF,-1);
            }
            else {
                if(cur_inst->reg_1 < AF){
                    regs[cur_inst->reg_1] = (regs[cur_inst->reg_1] + 1) & 0xFF;
                }
                else regs[cur_inst->reg_1] = (regs[cur_inst->reg_1] + 1);
                if ((opcode & 0x03) != 0x03) set_flags(regs[cur_inst->reg_1] == 0,0,(regs[cur_inst->reg_1] & 0xF) == 0xF,-1);
            }
            break;
        case JR:
            if(cond()){
                 pc += (char)fetch;
            }
            break;
        case RRCA: {
            uint8_t temp = regs[A] & 0x1;
            regs[A] = (regs[A] >> 1) || (temp << 7);
            set_flags(0, 0, 0, temp);
        }
            break;
        case RRA: {
            uint8_t temp = regs[A] & 0x1;
            regs[A] = (regs[A] >> 1) || (((regs[F] >> 4) & 0x1) << 7);
            set_flags(0, 0, 0, temp);
        }
            break;
        case AND:
            regs[A] &= fetch;
            set_flags(regs[A] == 0, 0, 1, 0);
            break;
        case OR:
            regs[A] |= (fetch & 0xFF);
            set_flags(regs[A] == 0,0,0,0);
            break;
        case CP: {
            int temp = (int) regs[A] - (int) fetch;
            set_flags(temp == 0, 1, ((int) regs[A] & 0xF) - ((int) fetch & 0xF) < 0, temp < 0);
        }
            break;
        case ADD:
            switch (opcode & 0xF0) {
                case 0x80:
                case 0xC0:
                    regs[cur_inst->reg_1] = (uint8_t)regs[cur_inst->reg_1] + (uint8_t)fetch;
                    set_flags(regs[cur_inst->reg_1] == 0,0,(regs[cur_inst->reg_1] & 0xF) + (fetch & 0xF) > 0xF,(int)regs[cur_inst->reg_1] + (int)fetch > 0xFF);
                    break;
                case 0xE0: {
                    signed char temp = (signed char) fetch;
                    regs[SP] += temp;
                    set_flags(0, 0, (regs[cur_inst->reg_1] & 0xF) + (fetch & 0xF) > 0xF,
                              (int)(regs[cur_inst->reg_1] & 0xFF) + (int)(fetch & 0xFF) > 0xFF);
                }
                    break;
                default:
                    regs[cur_inst->reg_1] += fetch;
                    set_flags(-1, 0, (regs[cur_inst->reg_1] & 0xFFF) + (fetch & 0xFFF) > 0xFFF,
                              (uint32_t) regs[cur_inst->reg_1] + (uint32_t) fetch > 0xFFFF);
            }
            break;
        case ADC:{
            uint16_t a = regs[A];
            regs[A] = ((uint8_t)regs[A] + (uint8_t)fetch + (uint8_t)((regs[F] >> 4) & 0x1));
            set_flags(regs[A] == 0, 0, (a & 0xF) + (fetch & 0xF) + ((regs[F] >> 4) & 0x1) > 0xF, a+fetch+((regs[F] >> 4) & 0x1) > 0xFF);
            }
            break;
        case SUB:
            set_flags(regs[A] - fetch == 0, 1, ((int)regs[A] & 0xF) - ((int)fetch & 0xF) < 0,
                      (int)regs[A] - (int)fetch < 0);
            regs[A] -= (uint8_t)fetch;
            break;
        case SBC:
            set_flags(regs[A] - fetch - ((regs[F] >> 4) & 0x1) == 0, 1,
                      ((int)regs[A] & 0xF) - ((int)fetch & 0xF) - (int)((regs[F] >> 4) & 0x1) < 0,
                      (int)regs[A] - (int)fetch - (int)((regs[F] >> 4) & 0x1) < 0);
            regs[A] -= ((uint8_t)fetch + (uint8_t)((regs[F] >> 4) & 0x1));
            break;
        case RLCA: {
            uint8_t temp = (regs[A] >> 7) & 0x1;
            regs[A] = (regs[A] << 1) || temp;
            set_flags(0,0,0,temp);
            }
            break;
        case RLA: {
            uint8_t temp = (regs[A] >> 7) & 0x1;
            regs[A] = (regs[A] << 1) || ((regs[F] >> 4) & 0x1);
            set_flags(0, 0, 0, temp);
            }
            break;
        case DI:
            master_enabled = false;
            break;
        case EI:
            ime = true;
            break;
        case POP:
            if(cur_inst->reg_1 == AF) regs[AF] = pop() & 0xFFF0;
            else regs[cur_inst->reg_1] = pop();
            break;
        case PUSH:
            push(regs[cur_inst->reg_1]);
            break;
        case CALL:
            if(cond()){
                push(pc);
                pc = fetch;
            }
            break;
        case RET:
            if(cond()) pc = pop();
            break;
        case RETI:
            master_enabled = true;
            pc = pop();
            break;
        case RST:
            push(pc);
            pc = bus_read(cur_inst->param);
            break;
        case DAA:{
            uint8_t u = 0;
            int fc = 0;
            if(((regs[F] >> 5) & 0x1) || (!((regs[F] >> 6) & 0x1) && (regs[A] & 0xF) > 9)) u = 6;
            if(((regs[F] >> 4) & 0x1) || (!((regs[F] >> 6) & 0x1) && regs[A] > 99)){
                u |= 0x60;
                fc = 1;
            }
            if((regs[F] >> 6) & 0x1) regs[A] += -u;
            else regs[A] += u;
            set_flags(regs[A] == 0, -1, 0, fc);
        }
        break;
        case CPL:
            regs[A] = ~regs[A];
            set_flags(-1,1,1,-1);
            break;
        case SCF:
            set_flags(-1,0,0,1);
            break;
        case CCF:
            set_flags(-1,0,0,((regs[F] >> 4) & 0x1) ^ 1);
            break;
        case HALT:
            halt = true;
            break;
        case CB:
            cb();
            break;
        default:
            printf("Unknown type");
            exit(-3);
            break;
    }
}

void cpu::set_flags(uint8_t z, uint8_t n, uint8_t h, uint8_t c){
    if(z >= 0) BIT_SET(7,z);
    if(n >= 0) BIT_SET(6,n);
    if(h >= 0) BIT_SET(5,h);
    if(c >= 0) BIT_SET(4,c);
}

void cpu::correct_regs(){
    if(regs[A] != saved_regs[A] || regs[F] != saved_regs[F]) regs[AF] = regs[A] << 8 | regs[F];
    if(regs[B] != saved_regs[B] || regs[C] != saved_regs[C]) regs[BC] = regs[B] << 8 | regs[C];
    if(regs[D] != saved_regs[D] || regs[E] != saved_regs[E]) regs[DE] = regs[D] << 8 | regs[E];
    if(regs[H] != saved_regs[H] || regs[L] != saved_regs[L]) regs[HL] = regs[H] << 8 | regs[L];

    if(regs[BC] != saved_regs[BC]){
        regs[B] = regs[BC] >> 8;
        regs[C] = regs[BC] & 0x00FF;
    }
    else if(regs[DE] != saved_regs[DE]){
        regs[D] = regs[DE] >> 8;
        regs[E] = regs[DE] & 0x00FF;
    }
    else if(regs[HL] != saved_regs[HL]){
        regs[H] = regs[HL] >> 8;
        regs[L] = regs[HL] & 0x00FF;
    }
    else if(regs[AF] != saved_regs[AF]){
        regs[A] = regs[AF] >> 8;
        regs[F] = regs[AF] & 0x00FF;
    }
}

uint8_t cpu::get_ie_register(){
    return ie_register;
}
void cpu::set_ie_register(uint8_t val){
    ie_register = val;
};

void cpu::push(uint16_t val){
    regs[SP]--;
    bus_write(regs[SP],(val >> 8) & 0xFF);
    regs[SP]--;
    bus_write(regs[SP],val & 0xFF);
}

uint16_t cpu::pop(){
    uint8_t low = bus_read(regs[SP]++);
    uint8_t high = bus_read(regs[SP]++);

    return (high << 8) | low;
}

void cpu::check_interrupt(){
    if (check_int(0x40, 1)){}
    else if (check_int(0x48, 2)){}
    else if (check_int(0x50, 4)){}
    else if (check_int(0x58, 8)){}
    else if (check_int(0x60, 16)){}
}

bool cpu::check_int(uint16_t addr, uint8_t interrupt){
    if((int_flags & interrupt) && (ie_register & interrupt)){
        push(pc);
        pc = addr;
        int_flags &= ~interrupt;
        halt = false;
        master_enabled = false;
        return true;
    }
    return false;
}

void cpu::cb(){
    uint8_t cb_op = fetch;
    if((cb_op & 0xF) == 0 || (cb_op & 0xF) == 8) cb_reg = B;
    else if((cb_op & 0xF) == 1 || (cb_op & 0xF) == 9) cb_reg = C;
    else if((cb_op & 0xF) == 2 || (cb_op & 0xF) == 10) cb_reg = D;
    else if((cb_op & 0xF) == 3 || (cb_op & 0xF) == 11) cb_reg = E;
    else if((cb_op & 0xF) == 4 || (cb_op & 0xF) == 12) cb_reg = H;
    else if((cb_op & 0xF) == 5 || (cb_op & 0xF) == 13) cb_reg = L;
    else if((cb_op & 0xF) == 6 || (cb_op & 0xF) == 14) cb_reg = HL;
    else if((cb_op & 0xF) == 7 || (cb_op & 0xF) == 15) cb_reg = A;

    if(cb_reg == HL) cb_fetch = bus_read(regs[HL]);
    else cb_fetch = regs[cb_reg];

    uint8_t val;

    if(cb_op < 0x08){
        set_flags(((cb_fetch << 1) & 0xFF) == 0,0,0,(cb_fetch >> 7) & 0x1);
        val = (cb_fetch << 1) | ((cb_fetch >> 7) & 0x1);
    }
    else if(cb_op < 0x10){
        val = (cb_fetch >> 1) | (cb_fetch << 7);
        set_flags(val == 0,0,0,cb_fetch & 0x1);
    }
    else if(cb_op < 0x18){
        val = (cb_fetch << 1) | ((regs[F] >> 4) & 0x1);
        set_flags(val == 0,0,0,(cb_fetch >> 7) & 0x1);
    }
    else if(cb_op < 0x20){
        val = (cb_fetch >> 1) | (((regs[F] >> 4) & 0x1) << 7);
        set_flags(val == 0,0,0,cb_fetch & 0x1);
    }
    else if(cb_op < 0x28){
        val = cb_fetch << 1;
        set_flags(val == 0,0,0,(cb_fetch >> 7) & 0x1);
    }
    else if(cb_op < 0x30){
        val = (cb_fetch >> 1) | (cb_fetch & 0x80);
        set_flags(val == 0,0,0,cb_fetch & 0x1);
    }
    else if(cb_op < 0x38){
        val = ((cb_fetch & 0xF0) >> 4) | ((cb_fetch & 0xF) << 4);
        set_flags(val == 0,0,0,0);
    }
    else if(cb_op < 0x40){
        val = cb_fetch >> 1;
        set_flags(val == 0,0,0,cb_fetch & 0x1);
    }
    else if(cb_op < 0x80){
        uint8_t bit = (cb_op >> 3) & 0b111;
        set_flags(!(cb_fetch & (1 << bit)), 0, 1, -1);
    }
    else if(cb_op < 0xC0){
        uint8_t bit = (cb_op >> 3) & 0b111;
        val = cb_fetch & ~(1 << bit);
    }
    else if(cb_op <= 0xFF){
        uint8_t bit = (cb_op >> 3) & 0b111;
        val = cb_fetch | (1 << bit);
    }


    if(cb_op < 0x40 || cb_op >= 0x80){
        if(cb_reg == HL) bus_write(regs[HL],val);
        else regs[cb_reg] = val;
    }
}
