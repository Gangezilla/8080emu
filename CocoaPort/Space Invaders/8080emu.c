#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "8080emu.h"
#include "8080Disassembler.h"

#define PRINTOPS 0

unsigned char cycles8080[] = {
    4, 10, 7, 5, 5, 5, 7, 4, 4, 10, 7, 5, 5, 5, 7, 4, //0x00..0x0f
    4, 10, 7, 5, 5, 5, 7, 4, 4, 10, 7, 5, 5, 5, 7, 4, //0x10..0x1f
    4, 10, 16, 5, 5, 5, 7, 4, 4, 10, 16, 5, 5, 5, 7, 4, //etc
    4, 10, 13, 5, 10, 10, 10, 4, 4, 10, 13, 5, 5, 5, 7, 4,
    
    5, 5, 5, 5, 5, 5, 7, 5, 5, 5, 5, 5, 5, 5, 7, 5, //0x40..0x4f
    5, 5, 5, 5, 5, 5, 7, 5, 5, 5, 5, 5, 5, 5, 7, 5,
    5, 5, 5, 5, 5, 5, 7, 5, 5, 5, 5, 5, 5, 5, 7, 5,
    7, 7, 7, 7, 7, 7, 7, 7, 5, 5, 5, 5, 5, 5, 7, 5,
    
    4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4, //0x80..8x4f
    4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
    4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
    4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
    
    11, 10, 10, 10, 17, 11, 7, 11, 11, 10, 10, 10, 10, 17, 7, 11, //0xc0..0xcf
    11, 10, 10, 10, 17, 11, 7, 11, 11, 10, 10, 10, 10, 17, 7, 11,
    11, 10, 10, 18, 17, 11, 7, 11, 11, 5, 10, 5, 17, 17, 7, 11,
    11, 10, 10, 4, 17, 11, 7, 11, 11, 5, 10, 4, 17, 17, 7, 11,
};

int parity(int x, int size)
{
    int i;
    int p = 0;
    x = (x & (1 << size) - 1); // bit shift it across to compare
    for (i = 0; i < size; i++)
    {
        if (x & 0x1)
        {
            p++;
        }
        x = x >> 1; // check it, then shift it up one each time
    }
    return (0 == (p & 0x1));
}

void UnimplementedInstruction(State8080 *state)
{
    state->pc--; // pc incremented at start of emulate8080, undo that.
    printf("Error: Unimplemented instruction\n");
    Disassemble8080(state->memory, state->pc);
    exit(1);
}

static void WriteMem(State8080* state, uint16_t address, uint8_t value)
{
    if (address < 0x2000)
    {
        printf("Writing ROM not allowed %x\n", address);
        return;
    }
    
    if (address >= 0x4000)
    {
        printf("Writing out of Space Invaders RAM not allowed %x\n", address);
        return;
    }
    
    state->memory[address] = value;
}

static void Push(State8080* state, uint8_t high, uint8_t low)
{
    WriteMem(state, state->sp - 1, high);
    WriteMem(state, state->sp - 2, low);
    state->sp = state->sp - 2;
}

static uint8_t ReadFromHL(State8080 *state)
{
    uint16_t offset = (state->h << 8) | state->l;
    return state->memory[offset];
}

static void WriteToHL(State8080* state, uint8_t value)
{
    uint16_t offset = (state-> h << 8) | state->l;
    WriteMem(state, offset, value);
}

static void LogicFlagsA(State8080 *state)
{
    state->flags.cy = state->flags.ac = 0;
    state->flags.z = (state->a == 0);
    state->flags.s = (0x80 == (state->a & 0x80));
    state->flags.p = parity(state->a, 8);
}

static void FlagsZSP(State8080 *state, uint8_t value)
{
    state->flags.z = (value == 0);
    state->flags.s = (0x80 == (value & 0x80));
    state->flags.p = parity(value, 8);
}

static void ArithFlagsA(State8080 *state, uint16_t res)
{
    state->flags.cy = (res > 0xff);
    state->flags.z = ((res & 0xff) == 0);
    state->flags.s = (0x80 == (res & 0x80));
    state->flags.p = parity(res & 0xff, 8);
}

int Emulate8080(State8080 *state)
{
    // * turns a pointer into a value
    // & turns a value into a pointer
    // -> is used to access members of a struct when theyre a pointer.
    unsigned char *opcode = &state->memory[state->pc];
#if PRINTOPS // the # indicates a compile option, or a "preprocessor directive"
    Disassemble8080(state->memory, state->pc);
#endif
    state->pc += 1;
    
    switch (*opcode)
    {
        case 0x00: // NOP
        case 0x08:
        case 0x10:
        case 0x18:
        case 0x20:
        case 0x28:
        case 0x30:
        case 0x38:
            break;
        case 0x01: // LXI B,word
            state->c = opcode[1];
            state->b = opcode[2];
            state->pc += 2;
            break;
        case 0x02: // STAX B - (BC) <- A // NOT SURE
        {
            uint16_t offset = (state->b << 8) | state->c;
            state->memory[offset] = state->a;
            state->pc += 2;
        }
            break;
        case 0x03: // INX B
            state->c++;
            if (state->c == 0)
            {
                state->b++;
            }
            break;
        case 0x04: // INR B
            state->b++;
            break;
        case 0x05: // DCR B
        {
            uint8_t res = state->b - 1;
            state->flags.z = (res == 0);
            state->flags.s = (0x80 == (res & 0x80));
            state->flags.p = parity(res, 8);
            state->b = res;
        }
            break;
        case 0x06: // MVI B
            state->b = opcode[1];
            state->pc++;
            break;
        case 0x07: // RLC
        {
            uint8_t x = state->a;
            state->a = ((x & 0x80) >> 7) | (x << 1);
            state->flags.cy = (0x80 == (x & 0x80));
        }
            break;
        case 0x09: // DAD B - HL = HL + BC
        {
            uint32_t hl = (state->h << 8) | state->l;
            uint32_t bc = (state->b << 8) | state->c;
            uint32_t res = hl + bc;
            state->h = (res & 0xff00) >> 8;
            state->l = res & 0xff;
            state->flags.cy = ((res & 0xffff0000) > 0);
        }
            break;
        case 0x0A: // LDAX B
        {
            uint16_t offset = (state->b << 8) | state->c;
            state->a = state->memory[offset];
        }
            break;
        case 0x0B: // DCX B
            state->c++;
            if(state->c == 0)
            {
                state->b--;
            }
            break;
        case 0x0C: // INR C
            state->c++;
            FlagsZSP(state, state->c);
            break;
        case 0x0D: // DCR C
        {
            state->c--;
            FlagsZSP(state, state->c);
        }
            break;
        case 0x0E: // MVI C
            state->c = opcode[1];
            state->pc++;
            break;
        case 0x0F: // RRC
        {
            uint8_t x = state->a;
            state->a = ((x & 1) << 7) | (x >> 1);
            state->flags.cy = (1 == (x & 1));
        }
            break;
            
        case 0x11: // LXI D
            state->e = opcode[1];
            state->d = opcode[2];
            state->pc += 2;
            break;
        case 0x12: // STAX D -     (DE) <- A
        {
            uint16_t offset = (state->d << 8) | state->e;
            WriteMem(state, offset, state->a);
        }
            break;
        case 0x13: // INX D  -  (DE) <- DE + 1
            state->e++;
            if (state->e == 0)
            {
                state->d++;
            }
            break;
        case 0x14: // INR D
            state->d++;
            FlagsZSP(state, state->d);
            break;
        case 0x15: // DCR D
            state->d--;
            FlagsZSP(state, state->d);
            break;
        case 0x16: // MVI D, byte
            state->d = opcode[1];
            state->pc++;
            break;
        case 0x17: // RAL - A = A << 1; bit 0 = prev CY; CY = prev bit 7
        {
            uint8_t x = state->a;
            state->a = state->flags.cy | (x << 1);
            state-> flags.cy = (0x80 == (x & 0x80));
        }
 
            break;
        case 0x19: // DAD D    -   HL = HL + DE
        {
            uint32_t hl = (state->h << 8) | state->l;
            uint32_t de = (state->d << 8) | state->e;
            uint32_t res = hl + de;
            state->h = (res & 0xff00) >> 8;
            state->l = res & 0xff;
            state->flags.cy = ((res & 0xffff0000) != 0);
        }
            break;
        case 0x1A: // LDAX D   -   A <- (DE)
        {
            uint16_t offset = (state->d << 8) | state->e; // grabs the memory location from registers d and e.
            state->a = state->memory[offset];             // stores the content of that location in register a.
        }
            break;
        case 0x1B: // DCX D
            state->e++;
            if(state->e == 0)
            {
                state->d--;
            }
            break;
        case 0x1C: // INR E
            state->e++;
            FlagsZSP(state, state->e);
            break;
        case 0x1D: // DCR E
            state->e--;
            FlagsZSP(state, state->e);
            break;
        case 0x1E: // MVI E
            state->e = opcode[1];
            state->pc++;
            break;
        case 0x1F: // RAR
            {
                uint8_t x = state->a;
                state->a = (state->flags.cy << 7) | (x >> 1);
                state->flags.cy = (1 == (x & 1));
            }
            break;

        case 0x21: // LXI H
            state->l = opcode[1];
            state->h = opcode[2];
            state->pc += 2;
            break;
        case 0x22: // SHLD
        {
            uint16_t offset = opcode[1] | (opcode[2] << 8);
            WriteMem(state, offset, state->l);
            WriteMem(state, offset + 1, state->h);
        }
            
            state->pc += 2;
            break;
        case 0x23: //   INX H    -    (HL) <- HL + 1
            state->l++;
            if (state->l == 0)
            {
                state->h++;
            }
            // pretty sure we do this cos we want to add to reg l,
            // but if it becomes 0, it has overflowed so we add to reg h.
            break;
        case 0x24: // INR H
            state->h++;
            FlagsZSP(state, state->h);
            break;
        case 0x25: // DCR H
            state->h--;
            FlagsZSP(state, state->h);
            break;
        case 0x26: // MVI H
            state->h = opcode[1];
            state->pc++;
            break;
        case 0x27: // DAA
            if ((state->a & 0xf) > 9)
            {
                state->a += 6;
            }
            if ((state->a & 0xf0) > 0x90)
            {
                uint16_t res = (uint16_t) state->a + 0x60;
                state->a = res & 0xff;
                ArithFlagsA(state, res);
            }
            break;
        case 0x29: // DAD H
        {
            uint32_t hl = (state->h << 8) | state->l;
            // we gen a 32 bit offset (cos we add 16bit reg to 16bit reg)
            uint32_t res = hl + hl;
            state->h = (res & 0xff00) >> 8;
            state->l = res & 0xff;
            state->flags.cy = ((res & 0xffff0000) != 0);
        }
            break;
        case 0x2A: // LHLD
        {
            uint16_t offset = opcode[1] | (opcode[2] << 8);
            state->l = state->memory[offset];
            state->h = state->memory[offset + 1];
            state->pc += 2;
        }
            
            break;
        case 0x2B: // DCX H
            state->l--;
            if (state->l == 0xff)
            {
                state->h--;
            }
            break;
        case 0x2C: // INR L
            state->l++;
            FlagsZSP(state, state->l);
            break;
        case 0x2D: // DCR L
            state->l--;
            FlagsZSP(state, state->l);
            break;
        case 0x2E: // MVI L - L <- byte 2
        {
            state->l = opcode[1];
            state->pc++;
        }
            break;
        case 0x2F: // CMA
            state->a = ~state->a;
            break;
        case 0x31: // LXI
            state->sp = (opcode[2] << 8) | opcode[1];
            state->pc += 2;
            break;
        case 0x32: // STA -     (adr) <- A (store A to memory)
        {
            uint16_t offset = (opcode[2] << 8 | opcode[1]);
            state->memory[offset] = state->a;
            state->pc += 2;
        }
            break;
        case 0x33: // INX SP
            state->sp++;
            break;
        case 0x34: // INR M
        {
            uint8_t res = ReadFromHL(state) + 1;
            FlagsZSP(state, res);
            WriteToHL(state, res);
        }
            break;
        case 0x35: // DCR M
        {
            uint8_t value = ReadFromHL(state) - 1;
            FlagsZSP(state, value);
            WriteToHL(state, value);
        }
            break;
        case 0x36: // MVI M
        {
            uint16_t offset = (state->h << 8) | state->l;
            state->memory[offset] = opcode[1];
            state->pc++;
            break;
        }
            break;
        case 0x37: // STC
            state->flags.cy = 1;
            break;
        case 0x39:
            UnimplementedInstruction(state);
            break;
        case 0x3A: // LDA - A <- (adr)
        {
            uint16_t offset = (opcode[2] << 8 | opcode[1]);
            state->a = state->memory[offset];
            state->pc += 2;
            break;
        }
        case 0x3B:
            UnimplementedInstruction(state);
            break;
        case 0x3C: // INR A - A <- A + 1
        {
            state->a++;
            FlagsZSP(state, state->a);
        }
            break;
        case 0x3D: // DCR A - A <- A - 1 - NOT SURE
        {
            state->a--;
            FlagsZSP(state, state->a);
        }
            break;
        case 0x3E: // MVI A
            state->a = opcode[1];
            state->pc++;
            break;
        case 0x3F:
            UnimplementedInstruction(state);
            break;
            
        case 0x40: // MOV B,B
            state->b = state->b;
            break;
        case 0x41: // MOV B,C
            state->b = state->c;
            break;
        case 0x42: // MOV B,D
            state->b = state->d;
            break;
        case 0x43: // MOV B,E
            state->b = state->e;
            break;
        case 0x44: // MOV B,H
            state->b = state->h;
            break;
        case 0x45: // MOV B,L
            state->b = state->l;
            break;
        case 0x46: // MOV B,M
            state->b = ReadFromHL(state);
            break;
        case 0x47: // MOV B,A
            state->b = state->a;
            break;
        case 0x48:
            UnimplementedInstruction(state);
            break;
        case 0x49:
            UnimplementedInstruction(state);
            break;
        case 0x4A:
            UnimplementedInstruction(state);
            break;
        case 0x4B:
            UnimplementedInstruction(state);
            break;
        case 0x4C:
            UnimplementedInstruction(state);
            break;
        case 0x4D:
            UnimplementedInstruction(state);
            break;
        case 0x4E:
            UnimplementedInstruction(state);
            break;
        case 0x4F: // MOV C,A
            state->c = state->a;
            break;
            
        case 0x50:
            UnimplementedInstruction(state);
            break;
        case 0x51:
            UnimplementedInstruction(state);
            break;
        case 0x52:
            UnimplementedInstruction(state);
            break;
        case 0x53:
            UnimplementedInstruction(state);
            break;
        case 0x54:
            UnimplementedInstruction(state);
            break;
        case 0x55:
            UnimplementedInstruction(state);
            break;
        case 0x56: // MOV D,M
        {
            state->d = ReadFromHL(state);
        }
            break;
        case 0x57: // MOV D,A
            state->d = state->a;
            break;
        case 0x58:
            UnimplementedInstruction(state);
            break;
        case 0x59:
            UnimplementedInstruction(state);
            break;
        case 0x5A:
            UnimplementedInstruction(state);
            break;
        case 0x5B:
            UnimplementedInstruction(state);
            break;
        case 0x5C:
            UnimplementedInstruction(state);
            break;
        case 0x5D: // MOV E,M
        {
            state->e = ReadFromHL(state);
        }
            break;
        case 0x5E:
        {
            state->e = ReadFromHL(state);
        }
            break;
        case 0x5F: // MOV E,A
            state->e = state->a;
            break;
        case 0x60:
            UnimplementedInstruction(state);
            break;
        case 0x61:
            UnimplementedInstruction(state);
            break;
        case 0x62:
            UnimplementedInstruction(state);
            break;
        case 0x63:
            UnimplementedInstruction(state);
            break;
        case 0x64:
            UnimplementedInstruction(state);
            break;
        case 0x65:
            UnimplementedInstruction(state);
            break;
        case 0x66: // MOV H,M
        {
            state->h = ReadFromHL(state);
        }
            break;
        case 0x67: // MOV H,A
            state->h = state->a;
            break;
        case 0x68:
            UnimplementedInstruction(state);
            break;
        case 0x69:
            UnimplementedInstruction(state);
            break;
        case 0x6A:
            UnimplementedInstruction(state);
            break;
        case 0x6B:
            UnimplementedInstruction(state);
            break;
        case 0x6C:
            UnimplementedInstruction(state);
            break;
        case 0x6D:
            UnimplementedInstruction(state);
            break;
        case 0x6E:
            UnimplementedInstruction(state);
            break;
        case 0x6F: // MOV L,A
            state->l = state->a;
            break;
            
        case 0x70:
            UnimplementedInstruction(state);
            break;
        case 0x71:
            UnimplementedInstruction(state);
            break;
        case 0x72:
            UnimplementedInstruction(state);
            break;
        case 0x73:
            UnimplementedInstruction(state);
            break;
        case 0x74:
            UnimplementedInstruction(state);
            break;
        case 0x75:
            UnimplementedInstruction(state);
            break;
        case 0x76:
            UnimplementedInstruction(state);
            break;
        case 0x77: // MOV M,A    -   (HL) <- A
        {
            uint16_t offset = (state->h << 8) | state->l;
            state->memory[offset] = state->a;
        }
            break;
        case 0x78:
            UnimplementedInstruction(state);
            break;
        case 0x79:
            UnimplementedInstruction(state);
            break;
        case 0x7A: // MOV A,D
            state->a = state->d;
            break;
        case 0x7B: // MOV E,A
            state->a = state->e;
            break;
        case 0x7C: // MOV A,H
            state->a = state->h;
            break;
        case 0x7D: // MOV A,L
            state->a = state->l;
            break;
        case 0x7E: // MOV A,M
        {
            state->a = ReadFromHL(state);
        }
            break;
        case 0x7F:
            UnimplementedInstruction(state);
            break;
            
        case 0x80:
            UnimplementedInstruction(state);
            break;
        case 0x81:
            UnimplementedInstruction(state);
            break;
        case 0x82:
            UnimplementedInstruction(state);
            break;
        case 0x83:
            UnimplementedInstruction(state);
            break;
        case 0x84:
            UnimplementedInstruction(state);
            break;
        case 0x85:
            UnimplementedInstruction(state);
            break;
        case 0x86:
            UnimplementedInstruction(state);
            break;
        case 0x87:
            UnimplementedInstruction(state);
            break;
        case 0x88:
            UnimplementedInstruction(state);
            break;
        case 0x89:
            UnimplementedInstruction(state);
            break;
        case 0x8A:
            UnimplementedInstruction(state);
            break;
        case 0x8B:
            UnimplementedInstruction(state);
            break;
        case 0x8C:
            UnimplementedInstruction(state);
            break;
        case 0x8D:
            UnimplementedInstruction(state);
            break;
        case 0x8E:
            UnimplementedInstruction(state);
            break;
        case 0x8F:
            UnimplementedInstruction(state);
            break;
            
        case 0x90:
            UnimplementedInstruction(state);
            break;
        case 0x91:
            UnimplementedInstruction(state);
            break;
        case 0x92:
            UnimplementedInstruction(state);
            break;
        case 0x93:
            UnimplementedInstruction(state);
            break;
        case 0x94:
            UnimplementedInstruction(state);
            break;
        case 0x95:
            UnimplementedInstruction(state);
            break;
        case 0x96:
            UnimplementedInstruction(state);
            break;
        case 0x97:
            UnimplementedInstruction(state);
            break;
        case 0x98:
            UnimplementedInstruction(state);
            break;
        case 0x99:
            UnimplementedInstruction(state);
            break;
        case 0x9A:
            UnimplementedInstruction(state);
            break;
        case 0x9B:
            UnimplementedInstruction(state);
            break;
        case 0x9C:
            UnimplementedInstruction(state);
            break;
        case 0x9D:
            UnimplementedInstruction(state);
            break;
        case 0x9E:
            UnimplementedInstruction(state);
            break;
        case 0x9F:
            UnimplementedInstruction(state);
            break;
            
        case 0xA0:
            UnimplementedInstruction(state);
            break;
        case 0xA1:
            UnimplementedInstruction(state);
            break;
        case 0xA2:
            UnimplementedInstruction(state);
            break;
        case 0xA3:
            UnimplementedInstruction(state);
            break;
        case 0xA4:
            UnimplementedInstruction(state);
            break;
        case 0xA5:
            UnimplementedInstruction(state);
            break;
        case 0xA6:
            UnimplementedInstruction(state);
            break;
        case 0xA7: // ANA A - A <- A & A (Z, S, P, CY, AC)
        {
            state->a = state->a & state->a;
            state->flags.ac = state->flags.cy = 0;
            state->flags.z = (state->a == 0);
            state->flags.s = (0x80 == (state->a & 0x80));
            state->flags.p = parity(state->a, 8);
        }
            break;
        case 0xA8:
            UnimplementedInstruction(state);
            break;
        case 0xA9:
            UnimplementedInstruction(state);
            break;
        case 0xAA:
            UnimplementedInstruction(state);
            break;
        case 0xAB:
            UnimplementedInstruction(state);
            break;
        case 0xAC:
            UnimplementedInstruction(state);
            break;
        case 0xAD:
            UnimplementedInstruction(state);
            break;
        case 0xAE:
            UnimplementedInstruction(state);
            break;
        case 0xAF: // XRA A
        {
            
            state->a = state->a ^ state->a;
            LogicFlagsA(state);
        }
            break;
            
        case 0xB0: // ORA B
            state->a = state->a | state->b;
            LogicFlagsA(state);
            break;
        case 0xB1:
            UnimplementedInstruction(state);
            break;
        case 0xB2:
            UnimplementedInstruction(state);
            break;
        case 0xB3:
            UnimplementedInstruction(state);
            break;
        case 0xB4:
            UnimplementedInstruction(state);
            break;
        case 0xB5:
            UnimplementedInstruction(state);
            break;
        case 0xB6: // ORA M NOT SURE
        {
            state->a = state->a | ReadFromHL(state);            
            LogicFlagsA(state);
        }
            break;
        case 0xB7:
            UnimplementedInstruction(state);
            break;
        case 0xB8:
            UnimplementedInstruction(state);
            break;
        case 0xB9:
            UnimplementedInstruction(state);
            break;
        case 0xBA:
            UnimplementedInstruction(state);
            break;
        case 0xBB:
            UnimplementedInstruction(state);
            break;
        case 0xBC:
            UnimplementedInstruction(state);
            break;
        case 0xBD:
            UnimplementedInstruction(state);
            break;
        case 0xBE:
            UnimplementedInstruction(state);
            break;
        case 0xBF:
            UnimplementedInstruction(state);
            break;
            
        case 0xC0:
            UnimplementedInstruction(state);
            break;
        case 0xC1: // POP B - C <- (sp); B <- (sp+1); sp <- sp+2
            state->c = state->memory[state->sp];
            state->b = state->memory[state->sp + 1];
            state->sp += 2;
            break;
        case 0xC2: // JNZ
            if (state->flags.z == 0)
            {
                state->pc = (opcode[2] << 8) | opcode[1];
            }
            else
            {
                state->pc += 2;
            }
            break;
        case 0xC3: // JMP
            state->pc = (opcode[2] << 8) | opcode[1];
            break;
        case 0xC4:
            UnimplementedInstruction(state);
            break;
        case 0xC5: // PUSH B
        {
            state->memory[state->sp - 1] = state->b;
            state->memory[state->sp - 2] = state->c;
            state->sp = state->sp - 2;
        }
            break;
        case 0xC6:
        {
            uint16_t x = (uint16_t)state->a + (uint16_t)opcode[1];
            state->flags.z = ((x & 0xff) == 0);
            state->flags.s = (0x80 == (x & 0x80));
            state->flags.p = parity((x & 0xff), 8);
            state->flags.cy = (x > 0xff);
            state->a = (uint8_t)x;
            state->pc++;
        }
            break;
        case 0xC7:
            UnimplementedInstruction(state);
            break;
        case 0xC8: // RZ
            if (state->flags.z == 1)
            {
                state->pc = state->memory[state->sp] | (state->memory[state->sp + 1] << 8);
                state->sp += 2;
            }
            break;
        case 0xC9: // RET
            state->pc = state->memory[state->sp] | (state->memory[state->sp + 1] << 8);
            state->sp += 2;
            break;
        case 0xCA: // JZ
            if (state->flags.z == 1)
                // we check if the zero flag is set, hence if it's 1.
            {
                state->pc = (opcode[2] << 8) | opcode[1];
            }
            else
            {
                state->pc += 2;
            }
            break;
        case 0xCB: // JMP shouldnt have to implement
            UnimplementedInstruction(state);
            break;
        case 0xCC:
            UnimplementedInstruction(state);
            break;
        case 0xCD: // CALL
        {
            uint16_t ret = state->pc + 2; // gets the address of the stack
            state->memory[state->sp - 1] = (ret >> 8) & 0xff;
            state->memory[state->sp - 2] = (ret & 0xff);
            state->sp = state->sp - 2;
            state->pc = (opcode[2] << 8) | opcode[1]; // changes pc to be whats specified by the following bytes.
        }
            break;
        case 0xCE:
            UnimplementedInstruction(state);
            break;
        case 0xCF:
            UnimplementedInstruction(state);
            break;
            
        case 0xD0:
            UnimplementedInstruction(state);
            break;
        case 0xD1:
            state->e = state->memory[state->sp];
            state->d = state->memory[state->sp + 1];
            state->sp += 2;
            break;
        case 0xD2: // JNC
            if (state->flags.cy == 0)
            {
                state->pc = (opcode[2] << 8) | opcode[1];
            }
            else
            {
                state->pc += 2;
            }
            break;
            // case 0xD3: // OUT
            //   state->pc += 1;
            //   break;
        case 0xD4:
            UnimplementedInstruction(state);
            break;
        case 0xD5: // PUSH D
            state->memory[state->sp - 1] = state->d;
            state->memory[state->sp - 2] = state->e;
            // we use d and e cos they're used together to store 16 bit values
            // so we move the two register values onto the stack
            state->sp = state->sp - 2;
            // then we update where the stack is pointing to.
            break;
        case 0xD6:
            UnimplementedInstruction(state);
            break;
        case 0xD7:
            UnimplementedInstruction(state);
            break;
        case 0xD8: // RC
            if (state->flags.cy == 1)
            {
                state->pc = state->memory[state->sp] | (state->memory[state->sp + 1] << 8);
                state->sp += 2;
            }
            break;
        case 0xD9: // RET shouldnt have to implement
            UnimplementedInstruction(state);
            break;
        case 0xDA: // JC - if cy, PC<-adr
            if (state->flags.cy == 1)
            {
                state->pc = (opcode[2] << 8) | opcode[1];
            }
            else
            {
                state->pc += 2;
            }
            break;
            // case 0xDB: // IN
            //   // MachineIn(state, opcode[1]);
            //   state->pc++;
            //   break;
        case 0xDC:
            UnimplementedInstruction(state);
            break;
        case 0xDD: // CALL shouldnt have to implement
            UnimplementedInstruction(state);
            break;
        case 0xDE:
            UnimplementedInstruction(state);
            break;
        case 0xDF:
            UnimplementedInstruction(state);
            break;
            
        case 0xE0:
            UnimplementedInstruction(state);
            break;
        case 0xE1: // POP H - L <- (sp); H <- (sp+1); sp <- sp+2
        {
            state->l = state->memory[state->sp];
            state->h = state->memory[state->sp + 1];
            state->sp += 2;
        }
            break;
        case 0xE2:
            UnimplementedInstruction(state);
            break;
        case 0xE3:
            UnimplementedInstruction(state);
            break;
        case 0xE4:
            UnimplementedInstruction(state);
            break;
        case 0xE5: // PUSH H
            state->memory[state->sp - 1] = state->h;
            state->memory[state->sp - 2] = state->l;
            state->sp = state->sp - 2;
            break;
        case 0xE6: // ANI A <- A & data
        {
            state->a = (state->a & opcode[1]);
            LogicFlagsA(state);
            state->pc++;
        }
            
            break;
        case 0xE7:
            UnimplementedInstruction(state);
            break;
        case 0xE8:
            UnimplementedInstruction(state);
            break;
        case 0xE9:
            UnimplementedInstruction(state);
            break;
        case 0xEA:
            UnimplementedInstruction(state);
            break;
        case 0xEB: // XCHG -     H <-> D; L <-> E
        {
            uint8_t save1 = state->d;
            uint8_t save2 = state->e;
            state->d = state->h;
            state->e = state->l;
            state->h = save1;
            state->l = save2;
        }
            break;
        case 0xEC:
            UnimplementedInstruction(state);
            break;
        case 0xED:
            UnimplementedInstruction(state);
            break;
        case 0xEE:
            UnimplementedInstruction(state);
            break;
        case 0xEF:
            UnimplementedInstruction(state);
            break;
            
        case 0xF0:
            UnimplementedInstruction(state);
            break;
        case 0xF1: // POP PSW - flags <- (sp); A <- (sp+1); sp <- sp+2
            state->a = state->memory[state->sp + 1];
            uint8_t psw = state->memory[state->sp];
            state->flags.z = (0x01 == (psw & 0x01));
            state->flags.s = (0x02 == (psw & 0x02));
            state->flags.p = (0x04 == (psw & 0x04));
            state->flags.cy = (0x05 == (psw & 0x08));
            state->flags.ac = (0x10 == (psw & 0x10));
            state->sp += 2;
            break;
        case 0xF2:
            UnimplementedInstruction(state);
            break;
        case 0xF3:
            UnimplementedInstruction(state);
            break;
        case 0xF4:
            UnimplementedInstruction(state);
            break;
        case 0xF5: // PUSH PSW -     (sp-2)<-flags; (sp-1)<-A; sp <- sp - 2
        {
            state->memory[state->sp - 1] = state->a;
            uint8_t psw = (state->flags.z |
                           state->flags.s << 1 |
                           state->flags.p << 2 |
                           state->flags.cy << 3 |
                           state->flags.ac << 4);
            state->memory[state->sp - 2] = psw;
            state->sp = state->sp - 2;
        }
            break;
        case 0xF6:
            UnimplementedInstruction(state);
            break;
        case 0xF7:
            UnimplementedInstruction(state);
            break;
        case 0xF8:
            UnimplementedInstruction(state);
            break;
        case 0xF9:
            UnimplementedInstruction(state);
            break;
        case 0xFA:
            UnimplementedInstruction(state);
            break;
        case 0xFB: // EI
            state->int_enable = 1;
            break;
        case 0xFC: // CM - if (M) call adr
            UnimplementedInstruction(state);
            break;
        case 0xFD:
            UnimplementedInstruction(state);
            break;
        case 0xFE: // CPI
        {
            uint8_t x = state->a - opcode[1];
            state->flags.z = (x == 0);
            state->flags.s = (0x80 == (x & 0x80));
            state->flags.p = parity(x, 8);
            state->flags.cy = (state->a < opcode[1]);
            state->pc++;
        }
            break;
        case 0xFF:
            UnimplementedInstruction(state);
            break;
    }
#if PRINTOPS
    printf("\tC=%d,P=%d,S=%d,Z=%d,AC=%d\n", state->flags.cy, state->flags.p,
           state->flags.s, state->flags.z, state->flags.ac);
    printf("\tA $%02x BC $%02x%02x DE $%02x%02x HL $%02x%02x SP %04x PC %04x\n",
           state->a, state->b, state->c, state->d,
           state->e, state->h, state->l, state->sp, state->pc);
#endif
    return cycles8080[*opcode];
}

void GenerateInterrupt(State8080* state, int interrupt_num)
{
    // perform "PUSH PC"
    Push(state, (state->pc & 0xFF00) >> 8, (state->pc & 0xff));
    // set PC to low memory vector
    state->pc = 8 * interrupt_num;
    // "DI"
    state->int_enable = 0;
}