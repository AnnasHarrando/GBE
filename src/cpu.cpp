#include "cpu.h"
#include "cart.h"
#include "inst.h"
#include <iostream>
#include <cstdlib>

using namespace std;

void cpu::step(){

    uint16_t cur_pc = this->pc;
    saved_regs = regs;

    opcode = cart_read(this->pc++);
    printf("op: %02X, pc: %04X\n", opcode, cur_pc);
    printf("AF: %02X%02X, BC: %02X%02X, DE: %02X%02X, HL: %02X%02X\n",regs[A],regs[F],regs[B],regs[C],regs[D],regs[E],regs[H],regs[L]);

    cur_inst = get_instruction(opcode);
    printf("%s\n", inst_name(cur_inst->type));

    load_in_mem = 0;
    fetch_data();
    exec();
    correct_regs();
    if(this->pc == 500) exit(-2);
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
            fetch = cart_read(this->pc);
            this->pc++;
            break;
        case R_D16:
        case D16: {
            uint16_t low = cart_read(this->pc);
            this->pc++;
            uint16_t high = cart_read(this->pc);
            this->pc++;
            fetch = low | (high << 8);
        }
            break;
        case D16_R:
        case A16_R: {
            uint16_t low = cart_read(this->pc);
            this->pc++;
            uint16_t high = cart_read(this->pc);
            this->pc++;
            load_in_mem = low | (high << 8);
            fetch = regs[cur_inst->reg_2];
            }
            break;
        case R_A8:

        case MR:
            load_in_mem = regs[cur_inst->reg_1];
            break;
        case R_MR:
            if(cur_inst->reg_2 == C) fetch = regs[cur_inst->reg_2] |= 0xFF00;
            else fetch = regs[cur_inst->reg_2];
            break;
        case MR_R:
            load_in_mem = regs[cur_inst->reg_1];
            if(cur_inst->reg_1 == C) load_in_mem |= 0xFF00;
            fetch = regs[cur_inst->reg_2];
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
        case   I_NONE:
            printf("Unknown type   I_NONE");
            exit(-3);
            break;
        case NOP:
            break;
        case JP:
            printf("%i\n",fetch);
            if(cond()) this->pc = fetch;
            break;
        case XOR:
            regs[A] ^= fetch;
            set_flags(regs[A],0,0,0);
            break;
        case LD:
            if(load_in_mem != 0){
                if(opcode == 0x08){
                    cart_write(load_in_mem,regs[SP] & 0xFF);
                    cart_write(load_in_mem+1,regs[SP] >> 8);
                }
                cart_write(load_in_mem,fetch);
            }
            else regs[cur_inst->reg_1] = fetch;
            break;
        case DEC:
            if(load_in_mem != 0) {
                cart_write(load_in_mem, cart_read(load_in_mem)-1);
                set_flags(cart_read(load_in_mem) == 0,1,(cart_read(load_in_mem) & 0xF) == 0xF,-1);
            }
            else {
                regs[cur_inst->reg_1]--;
                if ((opcode & 0xB) != 0xB) set_flags(regs[cur_inst->reg_1] == 0,1,(regs[cur_inst->reg_1] & 0xF) == 0xF,-1);
            }
            break;
        case INC:
            if(load_in_mem != 0) {
                cart_write(load_in_mem, cart_read(load_in_mem)+1);
                set_flags(cart_read(load_in_mem) == 0,0,(cart_read(load_in_mem) & 0xF) == 0xF,-1);
            }
            else {
                regs[cur_inst->reg_1]++;
                if ((opcode & 0x3) != 0x3) set_flags(regs[cur_inst->reg_1] == 0,0,(regs[cur_inst->reg_1] & 0xF) == 0xF,-1);
            }
            break;
        case JR:
            if(cond()){
                this->pc += fetch;
            }
            break;
        case RRCA: {
            uint8_t temp = regs[A];
            regs[A] = (regs[A] >> 1) || ((regs[A] & 0x1) << 7);
            set_flags(0, 0, 0, temp);
        }
            break;
        case RRA: {
            uint8_t temp = regs[A] & 0x1;
            regs[A] = (regs[A] >> 1) || (regs[F] & 0x80);
            set_flags(0, 0, 0, temp);
        }
            break;
        case OR:
            if(load_in_mem != 0) regs[A] |= cart_read(load_in_mem);
            else regs[A] |= fetch;
            set_flags(regs[A],0,0,0);
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
                    regs[cur_inst->reg_1] += fetch;
                    set_flags(regs[cur_inst->reg_1] == 0,0,(regs[cur_inst->reg_1] & 0xF) + (fetch & 0xF) > 0x0F,(int)regs[cur_inst->reg_1] + (int)fetch > 0xFF);
                    break;
                case 0xE0: {
                    signed char temp = (uint8_t) fetch;
                    regs[SP] += temp;
                    set_flags(0, 0, (regs[cur_inst->reg_1] & 0xFF) + (fetch & 0xFF) > 0xFF,
                              (uint32_t) regs[cur_inst->reg_1] + (uint32_t) fetch > 0xFFFF);
                }
                    break;
                default:
                    regs[cur_inst->reg_1] += fetch;
                    set_flags(-1, 0, (regs[cur_inst->reg_1] & 0xFF) + (fetch & 0xFF) > 0xFF,
                              (uint32_t) regs[cur_inst->reg_1] + (uint32_t) fetch > 0xFFFF);
            }
            break;
        case RLCA: {
            uint8_t temp = regs[A] >> 7;
            regs[A] = (regs[A] << 1) || temp;
            set_flags(0,0,0,temp);
            }
            break;
        case RLA: {
            uint8_t temp = regs[A] >> 7;
            regs[A] = (regs[A] << 1) || (regs[F] & 0x80) >> 7;
            set_flags(0, 0, 0, temp);
            }
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

    if(regs[BC] == saved_regs[BC]){
        regs[B] = regs[BC] >> 8;
        regs[C] = regs[BC] & 0x00FF;
    }
    else if(regs[DE] == saved_regs[DE]){
        regs[D] = regs[DE] >> 8;
        regs[E] = regs[DE] & 0x00FF;
    }
    else if(regs[HL] == saved_regs[HL]){
        regs[H] = regs[HL] >> 8;
        regs[L] = regs[HL] & 0x00FF;
    }
}

