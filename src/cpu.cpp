#include "cpu.h"
#include "emu.h"
#include <cstdlib>
#include <fstream>

using namespace std;



FILE *fptr = nullptr;



void cpu::step(){

    if(!halt) {

        opcode = bus_read(pc++);
        cur_inst = get_instruction(opcode);

        memory_location = 0;
        fetch_data();

        exec();
    }
    else{
        cycles(1);

        if(int_flags){
            halt = false;
        }
    }

    if(ime) {
        check_interrupt();
    }

}

void cpu::fetch_data(){
    switch(cur_inst->mode){
        case IMP:
            break;
        case R:
            fetch = get_register(cur_inst->reg_1);
            break;
        case R_R:
            fetch = get_register(cur_inst->reg_2);
            break;
        case R_D8:
        case D8:
            fetch = bus_read( pc);
            cycles(1);
            pc++;
            break;
        case R_D16:
        case D16: {
            uint16_t low = bus_read( pc);
            cycles(1);
            pc++;
            uint16_t high = bus_read( pc);
            cycles(1);
            pc++;
            fetch = low | (high << 8);
        }
            break;
        case A16_R: {
            uint16_t low = bus_read( pc);
            cycles(1);
            pc++;
            uint16_t high = bus_read( pc);
            cycles(1);
            pc++;
            memory_location = low | (high << 8);
            fetch = get_register(cur_inst->reg_1);
        }
            break;
        case R_A8:
            fetch = bus_read( pc);
            cycles(1);
            pc++;
            break;
        case A8_R:
            memory_location = bus_read( pc) | 0xFF00;
            cycles(1);
            pc++;
            break;
        case R_A16:{
            uint16_t low = bus_read( pc);
            cycles(1);
            pc++;
            uint16_t high = bus_read( pc);
            cycles(1);
            pc++;
            fetch = bus_read(low | (high << 8));
            cycles(1);
        } break;
        case MR:
            memory_location = get_register(cur_inst->reg_1);
            cycles(1);
            break;
        case R_MR:
            if(cur_inst->reg_2 == C) fetch = bus_read(get_register(cur_inst->reg_2) | 0xFF00);
            else fetch = bus_read(get_register(cur_inst->reg_2));
            cycles(1);
            break;
        case MR_R:
            memory_location = get_register(cur_inst->reg_1);
            if(cur_inst->reg_1 == C) memory_location |= 0xFF00;
            fetch = get_register(cur_inst->reg_2);
            break;
        case MR_D8:
            memory_location = get_register(cur_inst->reg_1);
            fetch = bus_read( pc);
            cycles(1);
            pc++;
            break;
        case HLD_R:
            memory_location = regists.hl;
            fetch = regists.a;
            set_register(HL, get_register(HL) - 1);
            break;
        case HLI_R:
            memory_location = regists.hl;
            fetch = regists.a;
            set_register(HL, get_register(HL) + 1);
            break;
        case R_HLI:
            fetch = bus_read(regists.hl);
            cycles(1);
            regists.hl++;
            break;
        case R_HLD:
            fetch = bus_read(regists.hl);
            cycles(1);
            regists.hl--;
            break;
        case HL_SPR:{
            fetch = bus_read( pc);
            cycles(1);
            pc++;
        }
            break;
        default:
            printf("unknown mode %d (%02X)\n", cur_inst->mode, opcode);
            exit(-7);
    }
}


bool cpu::cond(){
    bool z = (regists.f >> 7) & 1;
    bool c = (regists.f >> 4) & 1;

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
            printf("Unknown type I_NONE");
            exit(-3);
        case NOP:
            break;
        case JP:
            if(cond()) {
                pc = fetch;
                cycles(1);
            }
            break;
        case XOR:
            regists.a ^= fetch;
            set_flags(regists.a == 0,0,0,0);
            break;
        case LD:
            if(opcode == 0xF8) {
                regists.hl = regists.sp + (signed char)fetch;
                cycles(1);
                set_flags(0,0,(get_register(SP) & 0xF) + (fetch & 0xF) > 0xF,(get_register(SP) & 0xFF) + (fetch & 0xFF) > 0xFF);
            }
            else{
                if(memory_location != 0){
                    if(opcode == 0x08){
                        bus_write(memory_location,regists.sp & 0xFF);
                        bus_write(memory_location+1,regists.sp >> 8);
                        cycles(1);
                    }
                    else bus_write(memory_location,fetch);
                    cycles(1);
                }
                else set_register(cur_inst->reg_1, fetch);

                if(cur_inst->mode == R_R && cur_inst->reg_2 >= AF) cycles(1);
            }
            break;
        case LDH:
            if(memory_location != 0) bus_write(memory_location,regists.a);
            else regists.a = bus_read(0xFF00 | fetch);
            cycles(1);
            break;
        case DEC:
            if(opcode == 0x35) {
                bus_write(memory_location, bus_read(memory_location)-1);
                cycles(1);
                set_flags(bus_read(memory_location) == 0,1,(bus_read(memory_location) & 0xF) == 0xF,-1);
            }
            else {
                set_register(cur_inst->reg_1, get_register(cur_inst->reg_1) - 1);
                if(cur_inst->reg_1 >= AF) cycles(1);
                else set_flags(get_register(cur_inst->reg_1) == 0,1,(get_register(cur_inst->reg_1) & 0xF) == 0xF,-1);
            }
            break;
        case INC:
            if(opcode == 0x34) {
                bus_write(memory_location, bus_read(memory_location)+1);
                cycles(1);
                set_flags(bus_read(memory_location) == 0,0,(bus_read(memory_location) & 0xF) == 0,-1);
            }
            else {
                set_register(cur_inst->reg_1, get_register(cur_inst->reg_1) + 1);
                if(cur_inst->reg_1 >= AF) cycles(1);
                else set_flags(get_register(cur_inst->reg_1) == 0,0,(get_register(cur_inst->reg_1) & 0xF) == 0,-1);
            }
            break;
        case JR:
            if(cond()){
                pc += (int8_t)(fetch & 0xFF);
                cycles(1);
            }
            break;
        case RRCA: {
            uint8_t temp = regists.a & 0x1;
            regists.a = (regists.a >> 1) | (temp << 7);
            set_flags(0, 0, 0, temp);
        }
            break;
        case RRA: {
            uint8_t temp = regists.a & 0x1;
            regists.a = (regists.a >> 1) | ((((regists.f >> 4) & 0x1) << 7) & 0xFF);
            set_flags(0, 0, 0, temp);
        }
            break;
        case AND:
            regists.a &= fetch;
            set_flags(regists.a == 0, 0, 1, 0);
            break;
        case OR:
            regists.a |= (fetch & 0xFF);
            set_flags(regists.a == 0,0,0,0);
            break;
        case CP: {
            int temp = (int) regists.a - (int) fetch;
            set_flags(temp == 0, 1, ((int) regists.a & 0xF) - ((int) fetch & 0xF) < 0, temp < 0);
        }
            break;
        case ADD:
            if(opcode == 0xE8){
                set_flags(0, 0, (regists.sp & 0xF) + (fetch & 0xF) > 0xF,
                          (int)(regists.sp & 0xFF) + (int)(fetch & 0xFF) > 0xFF);
                signed char temp = (signed char) fetch;
                cycles(1);
                regists.sp += temp;
            }
            else {
                if (cur_inst->reg_1 == HL) {
                    set_flags(-1, 0, (get_register(HL) & 0xFFF) + (fetch & 0xFFF) > 0xFFF,
                              (uint32_t) get_register(HL) + (uint32_t) fetch > 0xFFFF);
                    set_register(HL, get_register(HL) + fetch);
                    cycles(1);
                } else {
                    uint8_t temp = get_register(A);
                    set_register(A, get_register(A) + fetch);
                    set_flags(get_register(A) == 0,0,(temp & 0xF) + (fetch & 0xF) > 0xF,temp + fetch > 0xFF);
                }
            }
            break;
        case ADC:{
            uint8_t temp = get_register(A);
            set_register(A,temp+fetch+((regists.f >> 4) & 0x1));
            set_flags(regists.a == 0, 0, (temp & 0xF) + (fetch & 0xF) + ((regists.f >> 4) & 0x1) > 0xF, temp+(uint8_t)fetch+((regists.f >> 4) & 0x1) > 0xFF);
        }
            break;
        case SUB:
            set_flags(regists.a - fetch == 0, 1, ((int)regists.a & 0xF) - ((int)fetch & 0xF) < 0,
                      (int)regists.a - (int)fetch < 0);
            regists.a -= (uint8_t)fetch;
            break;
        case SBC: {
            uint8_t temp = get_register(A);
            set_register(A, temp - fetch - ((regists.f >> 4) & 0x1));
            set_flags(regists.a == 0, 1,
                      ((int) temp & 0xF) - ((int) fetch & 0xF) - (int) ((regists.f >> 4) & 0x1) < 0,
                      (int) temp - (int) fetch - (int) ((regists.f >> 4) & 0x1) < 0);
        }
            break;
        case RLCA: {
            set_flags(0,0,0, get_register(A) >> 7);
            uint8_t temp = (get_register(A) << 1) | (get_register(A) >> 7);
            set_register(A,temp);
        }
            break;
        case RLA: {
            uint8_t temp = (get_register(A) << 1) | ((get_register(F) >> 4) & 1);
            set_flags(0, 0, 0, (get_register(A) >> 7) & 0x1);
            set_register(A,temp);
        }
            break;
        case DI:
            ime = false;
            break;
        case EI:
            ime = true;
            break;
        case POP:
            if(cur_inst->reg_1 == AF) regists.af = pop() & 0xFFF0;
            else set_register(cur_inst->reg_1, pop());
            break;
        case PUSH:
            push(get_register(cur_inst->reg_1));
            cycles(1);
            break;
        case CALL:
            if(cond()){
                push(pc);
                pc = fetch;
                cycles(1);
            }
            break;
        case RET:
            if(cur_inst->cond != CT_NONE) cycles(1);
            if(cond()){
                pc = pop();
                cycles(1);
            }
            break;
        case RETI:
            ime = true;
            pc = pop();
            break;
        case RST:
            push(pc);
            pc = cur_inst->param;
            cycles(1);
            break;
        case DAA:{
            //printf("DAA used\n");
            int u = 0;
            int fc = 0;

            if(((get_register(F) >> 5) & 0x1) || (!((get_register(F) >> 6) & 0x1) && ((get_register(A) & 0xF) > 9))) u = 0x6;
            if(((get_register(F) >> 4) & 0x1) || (!((get_register(F) >> 6) & 0x1) && get_register(A) > 99)){
                u |= 0x60;
                fc = 1;
            }
            if((get_register(F) >> 6) & 0x1) set_register(A,get_register(A)-u);
            else set_register(A,get_register(A)+u);
            set_flags(get_register(A) == 0, -1, 0, fc);
            }
            break;
        case CPL:
            regists.a = ~regists.a;
            set_flags(-1,1,1,-1);
            break;
        case SCF:
            set_flags(-1,0,0,1);
            break;
        case CCF:
            set_flags(-1,0,0,((regists.f >> 4) & 0x1) ^ 1);
            break;
        case HALT:
            halt = true;
            break;
        case CB:
            cb();
            break;
        default:
            printf("Unknown type %02X",opcode);
            exit(-3);
    }
    cycles(1);

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

    cycles(1);

    if(cb_reg == HL) {
        cb_fetch = bus_read(regists.hl);
        cycles(2);
    }
    else cb_fetch = get_register(cb_reg);

    uint8_t val;

    if(cb_op < 0x08){
        val = (cb_fetch << 1) | ((cb_fetch >> 7) & 0x1);
        set_flags(val == 0,0,0,(cb_fetch >> 7) & 0x1);
    }
    else if(cb_op < 0x10){
        val = (cb_fetch >> 1) | (cb_fetch << 7);
        set_flags(val == 0,0,0,cb_fetch & 0x1);
    }
    else if(cb_op < 0x18){
        val = (cb_fetch << 1) | ((regists.f >> 4) & 0x1);
        set_flags(val == 0,0,0,(cb_fetch >> 7) & 0x1);
    }
    else if(cb_op < 0x20){
        val = (cb_fetch >> 1) | (((regists.f >> 4) & 0x1) << 7);
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
        if(cb_reg == HL) bus_write(regists.hl,val);
        else set_register(cb_reg, val);
    }
}

    void cpu::set_flags(int z, int n, int h, int c){
        if(z >= 0) BIT_SET(7,z);
        if(n >= 0) BIT_SET(6,n);
        if(h >= 0) BIT_SET(5,h);
        if(c >= 0) BIT_SET(4,c);
    }

void cpu::set_register(REG regis,uint16_t val) {
    switch(regis){
        case A: regists.a = val & 0xFF; break;
        case F: regists.f = val & 0xFF; break;
        case B: regists.b = val & 0xFF; break;
        case C: regists.c = val & 0xFF; break;
        case D: regists.d = val & 0xFF; break;
        case E: regists.e = val & 0xFF; break;
        case H: regists.h = val & 0xFF; break;
        case L: regists.l = val & 0xFF; break;

        case AF: regists.af = val & 0xFFF0; break;
        case BC: regists.bc = val; break;
        case DE: regists.de = val; break;
        case HL: regists.hl = val; break;
        case SP: regists.sp = val; break;
    }
}

uint16_t cpu::get_register(REG regis) {
    switch(regis) {
        case A:
            return (uint16_t)regists.a;
        case F:
            return (uint16_t)regists.f;
        case B:
            return (uint16_t)regists.b;
        case C:
            return (uint16_t)regists.c;
        case D:
            return (uint16_t)regists.d;
        case E:
            return (uint16_t)regists.e;
        case H:
            return (uint16_t)regists.h;
        case L:
            return (uint16_t)regists.l;

        case AF:
            return regists.af;
        case BC:
            return regists.bc;
        case DE:
            return regists.de;
        case HL:
            return regists.hl;
        case SP:
            return regists.sp;
    }
}

void cpu::push(uint16_t val){
    regists.sp--;
    bus_write(regists.sp,val >> 8);
    cycles(1);
    regists.sp--;
    bus_write(regists.sp,val & 0xFF);
    cycles(1);
}

uint16_t cpu::pop(){
    uint8_t low = bus_read(regists.sp);
    regists.sp++;
    cycles(1);
    uint8_t high = bus_read(regists.sp);
    regists.sp++;
    cycles(1);

    return low | (high << 8);
}

void cpu::check_interrupt(){
    if(check_int(0x40, 1)){

    }
    else if(check_int(0x48, 2)){

    }
    else if(check_int(0x50, 4)){

    }
    else if(check_int(0x58, 8)){

    }
    else if(check_int(0x60, 16)){

    }


}

bool cpu::check_int(uint16_t addr, uint8_t interrupt){
    if((int_flags & interrupt) && (ie_register & interrupt)){
        cycles(2);
        push(pc);
        pc = addr;
        cycles(1);
        int_flags &= ~interrupt;
        halt = false;
        ime = false;
        return true;
    }
    return false;
}

uint8_t cpu::get_ie_register(){
    return ie_register;
}

void cpu::set_ie_register(uint8_t val){
    ie_register = val;
}

void cpu::get_interrupt(uint8_t interrupt) {
    int_flags |= interrupt;
}