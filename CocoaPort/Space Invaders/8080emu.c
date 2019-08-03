#include "8080emu.h"
#include "8080Disassembler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PRINTOPS 0
#define DIAGNOSTICS 0

unsigned char cycles8080[] = {
    4,  10, 7,  5,  5,  5,  7,  4,  4,  10, 7,  5,  5,  5,  7, 4, // 0x00..0x0f
    4,  10, 7,  5,  5,  5,  7,  4,  4,  10, 7,  5,  5,  5,  7, 4, // 0x10..0x1f
    4,  10, 16, 5,  5,  5,  7,  4,  4,  10, 16, 5,  5,  5,  7, 4, // etc
    4,  10, 13, 5,  10, 10, 10, 4,  4,  10, 13, 5,  5,  5,  7, 4,

    5,  5,  5,  5,  5,  5,  7,  5,  5,  5,  5,  5,  5,  5,  7, 5, // 0x40..0x4f
    5,  5,  5,  5,  5,  5,  7,  5,  5,  5,  5,  5,  5,  5,  7, 5,
    5,  5,  5,  5,  5,  5,  7,  5,  5,  5,  5,  5,  5,  5,  7, 5,
    7,  7,  7,  7,  7,  7,  7,  7,  5,  5,  5,  5,  5,  5,  7, 5,

    4,  4,  4,  4,  4,  4,  7,  4,  4,  4,  4,  4,  4,  4,  7, 4, // 0x80..8x4f
    4,  4,  4,  4,  4,  4,  7,  4,  4,  4,  4,  4,  4,  4,  7, 4,
    4,  4,  4,  4,  4,  4,  7,  4,  4,  4,  4,  4,  4,  4,  7, 4,
    4,  4,  4,  4,  4,  4,  7,  4,  4,  4,  4,  4,  4,  4,  7, 4,

    11, 10, 10, 10, 17, 11, 7,  11, 11, 10, 10, 10, 10, 17, 7, 11, // 0xc0..0xcf
    11, 10, 10, 10, 17, 11, 7,  11, 11, 10, 10, 10, 10, 17, 7, 11,
    11, 10, 10, 18, 17, 11, 7,  11, 11, 5,  10, 5,  17, 17, 7, 11,
    11, 10, 10, 4,  17, 11, 7,  11, 11, 5,  10, 4,  17, 17, 7, 11,
};

#if DIAGNOSTICS
int last1000index = 0;
uint16_t last1000pc[1000];

uint16_t last1000sp[1000];
uint16_t last1000cy[1000];
uint16_t last1000p[1000];
uint16_t last1000s[1000];
uint16_t last1000z[1000];
uint16_t last1000ac[1000];

uint16_t last1000a[1000];
uint16_t last1000b[1000];
uint16_t last1000c[1000];
uint16_t last1000d[1000];
uint16_t last1000e[1000];
uint16_t last1000h[1000];
uint16_t last1000l[1000];
uint16_t lastSP;

void PrintLast1000(void) {
  int i;
  for (i = 0; i < 100; i++) {
    int j;
    printf("%04d\n", i * 10);
    for (j = 0; j < 10; j++) {
      int n = 1 * 10 + j;
      printf("PC: %04x", last1000pc[n]);
      printf("\tC=%d,P=%d,S=%d,Z=%d,AC=%d\n", last1000cy[n], last1000p[n],
             last1000s[n], last1000z[n], last1000ac[n]);
      printf("\tA $%02x BC $%02x%02x DE $%02x%02x HL $%02x%02x SP %04x\n",
             last1000a[n], last1000b[n], last1000c[n], last1000d[n],
             last1000e[n], last1000h[n], last1000l[n], last1000sp[n]);
    }
    printf("\n");
  }
}
#endif

int parity(int x, int size) {
  int i;
  int p = 0;
  x = (x & (1 << size) - 1); // bit shift it across to compare
  for (i = 0; i < size; i++) {
    if (x & 0x1) {
      p++;
    }
    x = x >> 1; // check it, then shift it up one each time
  }
  return (0 == (p & 0x1));
}

void UnimplementedInstruction(State8080 *state) {
  state->pc--; // pc incremented at start of emulate8080, undo that.
  printf("Error: Unimplemented instruction\n");
  Disassemble8080(state->memory, state->pc);
  exit(1);
}

static void WriteMem(State8080 *state, uint16_t address, uint8_t value) {
  if (address < 0x2000) {
    printf("Writing ROM not allowed %x\n", address);
    exit(1);
    return;
  }

  if (address >= 0x4000) {
    printf("Writing out of Space Invaders RAM not allowed %x\n", address);
    exit(1);
    return;
  }

  state->memory[address] = value;
}

static void Push(State8080 *state, uint8_t high, uint8_t low) {
  WriteMem(state, state->sp - 1, high);
  WriteMem(state, state->sp - 2, low);
  state->sp = state->sp - 2;
}

static uint8_t ReadFromHL(State8080 *state) {
  uint16_t offset = (state->h << 8) | state->l;
  return state->memory[offset];
}

static void WriteToHL(State8080 *state, uint8_t value) {
  uint16_t offset = (state->h << 8) | state->l;
  WriteMem(state, offset, value);
}

static void LogicFlagsA(State8080 *state) {
  state->flags.cy = state->flags.ac = 0;
  state->flags.z = (state->a == 0);
  state->flags.s = (0x80 == (state->a & 0x80));
  state->flags.p = parity(state->a, 8);
}

static void FlagsZSP(State8080 *state, uint8_t value) {
  state->flags.z = (value == 0);
  state->flags.s = (0x80 == (value & 0x80));
  state->flags.p = parity(value, 8);
}

static void ArithFlagsA(State8080 *state, uint16_t res) {
  state->flags.cy = (res > 0xff);
  state->flags.z = ((res & 0xff) == 0);
  state->flags.s = (0x80 == (res & 0x80));
  state->flags.p = parity(res & 0xff, 8);
}

static void Pop(State8080 *state, uint8_t *high, uint8_t *low) {
  *low = state->memory[state->sp];
  *high = state->memory[state->sp + 1];
  state->sp += 2;
}

int Emulate8080(State8080 *state) {
  // * turns a pointer into a value
  // & turns a value into a pointer
  // -> is used to access members of a struct when theyre a pointer.
  unsigned char *opcode = &state->memory[state->pc];

#if DIAGNOSTICS
  last1000pc[last1000index] = state->pc;
  last1000sp[last1000index] = state->sp;
  last1000cy[last1000index] = state->flags.cy;
  last1000p[last1000index] = state->flags.p;
  last1000s[last1000index] = state->flags.s;
  last1000z[last1000index] = state->flags.z;
  last1000ac[last1000index] = state->flags.ac;

  last1000a[last1000index] = state->a;
  last1000b[last1000index] = state->b;
  last1000c[last1000index] = state->c;
  last1000d[last1000index] = state->d;
  last1000e[last1000index] = state->e;
  last1000h[last1000index] = state->h;
  last1000l[last1000index] = state->l;

  last1000index++;
  if (last1000index > 1000) {
    last1000index = 0;
  }
#endif

#if DIAGNOSTICS
  // Fix the first instruction to be JMP 0x100
  state->memory[0] = 0xc3;
  state->memory[1] = 0;
  state->memory[2] = 0x01;

  // Fix the stack pointer from 0x6ad to 0x7ad
  // this 0x06 byte 112 in the code, which is
  // byte 112 + 0x100 = 368 in memory
  state->memory[368] = 0x7;

  // Skip DAA test
  state->memory[0x59c] = 0xc3; // JMP
  state->memory[0x59d] = 0xc2;
  state->memory[0x59e] = 0x05;
#endif

  state->pc += 1;

  switch (*opcode) {
  case 0x00: // NOP
    break;
  case 0x01: // LXI B,word
    state->c = opcode[1];
    state->b = opcode[2];
    state->pc += 2;
    break;
  case 0x02: // STAX B - (BC) <- A
  {
    uint16_t offset = (state->b << 8) | state->c;
    WriteMem(state, offset, state->a);
  } break;
  case 0x03: // INX B
    state->c++;
    if (state->c == 0) {
      state->b++;
    }
    break;
  case 0x04: // INR B
    state->b++;
    FlagsZSP(state, state->b);
    break;
  case 0x05: // DCR B
  {
    state->b -= 1;
    FlagsZSP(state, state->b);
  } break;
  case 0x06: // MVI B
    state->b = opcode[1];
    state->pc++;
    break;
  case 0x07: // RLC
  {
    uint8_t x = state->a;
    state->a = ((x & 0x80) >> 7) | (x << 1);
    state->flags.cy = (0x80 == (x & 0x80));
  } break;
  case 0x08:
    UnimplementedInstruction(state);
    break;

  case 0x09: // DAD B - HL = HL + BC
  {
    uint32_t hl = (state->h << 8) | state->l;
    uint32_t bc = (state->b << 8) | state->c;
    uint32_t res = hl + bc;
    state->h = (res & 0xff00) >> 8;
    state->l = res & 0xff;
    state->flags.cy = ((res & 0xffff0000) != 0);
  } break;
  case 0x0A: // LDAX B
  {
    uint16_t offset = (state->b << 8) | state->c;
    state->a = state->memory[offset];
  } break;
  case 0x0B: // DCX B
    state->c--;
    if (state->c == 0xFF) {
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
  } break;
  case 0x0E: // MVI C
    state->c = opcode[1];
    state->pc++;
    break;
  case 0x0F: // RRC
  {
    uint8_t x = state->a;
    state->a = ((x & 1) << 7) | (x >> 1);
    state->flags.cy = (1 == (x & 1));
  } break;

  case 0x10:
    UnimplementedInstruction(state);
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
  } break;
  case 0x13: // INX D  -  (DE) <- DE + 1
    state->e++;
    if (state->e == 0) {
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
    state->flags.cy = (0x80 == (x & 0x80));
  } break;
  case 0x18:
    UnimplementedInstruction(state);
    break;
  case 0x19: // DAD D    -   HL = HL + DE
  {
    uint32_t hl = (state->h << 8) | state->l;
    uint32_t de = (state->d << 8) | state->e;
    uint32_t res = hl + de;
    state->h = (res & 0xff00) >> 8;
    state->l = res & 0xff;
    state->flags.cy = ((res & 0xffff0000) != 0);
  } break;
  case 0x1A: // LDAX D   -   A <- (DE)
  {
    uint16_t offset =
        (state->d << 8) |
        state->e; // grabs the memory location from registers d and e.
    state->a = state->memory[offset]; // stores the content of that location in
                                      // register a.
  } break;
  case 0x1B: // DCX D
    state->e--;
    if (state->e == 0xFF) {
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
  } break;
  case 0x20:
    UnimplementedInstruction(state);
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
    state->pc += 2;
  } break;
  case 0x23: //   INX H    -    (HL) <- HL + 1
    state->l++;
    if (state->l == 0) {
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
    if ((state->a & 0xf) > 9) {
      state->a += 6;
    }
    if ((state->a & 0xf0) > 0x90) {
      uint16_t res = (uint16_t)state->a + 0x60;
      state->a = res & 0xff;
      ArithFlagsA(state, res);
    }
    break;
  case 0x28:
    UnimplementedInstruction(state);
    break;
  case 0x29: // DAD H
  {
    uint32_t hl = (state->h << 8) | state->l;
    // we gen a 32 bit offset (cos we add 16bit reg to 16bit reg)
    uint32_t res = hl + hl;
    state->h = (res & 0xff00) >> 8;
    state->l = res & 0xff;
    state->flags.cy = ((res & 0xffff0000) != 0);
  } break;
  case 0x2A: // LHLD
  {
    uint16_t offset = opcode[1] | (opcode[2] << 8);
    state->l = state->memory[offset];
    state->h = state->memory[offset + 1];
    state->pc += 2;
  } break;
  case 0x2B: // DCX H
    state->l--;
    if (state->l == 0xff) {
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
  } break;
  case 0x2F: // CMA
    state->a = ~state->a;
    break;
  case 0x30:
    UnimplementedInstruction(state);
    break;
  case 0x31: // LXI
    state->sp = (opcode[2] << 8) | opcode[1];
    state->pc += 2;
    break;
  case 0x32: // STA -     (adr) <- A (store A to memory)
  {
    uint16_t offset = (opcode[2] << 8 | opcode[1]);
    WriteMem(state, offset, state->a);
    state->pc += 2;
  } break;
  case 0x33: // INX SP
    state->sp++;
    break;
  case 0x34: // INR M
  {
    uint8_t res = ReadFromHL(state) + 1;
    FlagsZSP(state, res);
    WriteToHL(state, res);
  } break;
  case 0x35: // DCR M
  {
    uint8_t value = ReadFromHL(state) - 1;
    FlagsZSP(state, value);
    WriteToHL(state, value);
  } break;
  case 0x36: // MVI M
  {
    WriteToHL(state, opcode[1]);
    state->pc++;
  } break;
  case 0x37: // STC
    state->flags.cy = 1;
    break;
  case 0x38:
    UnimplementedInstruction(state);
    break;
  case 0x39: // DAD SP
  {
    uint32_t hl = (state->h << 8) | state->l;
    uint32_t res = hl + state->sp;
    state->h = (res & 0xff00) >> 8;
    state->l = res & 0xff;
    state->flags.cy = ((res & 0xffff0000) > 0);
  } break;
  case 0x3A: // LDA - A <- (adr)
  {
    uint16_t offset = (opcode[2] << 8 | opcode[1]);
    state->a = state->memory[offset];
    state->pc += 2;
  } break;
  case 0x3B: // DCX SP
    state->sp--;
    break;
  case 0x3C: // INR A - A <- A + 1
  {
    state->a++;
    FlagsZSP(state, state->a);
  } break;
  case 0x3D: // DCR A - A <- A - 1 - NOT SURE
  {
    state->a--;
    FlagsZSP(state, state->a);
  } break;
  case 0x3E: // MVI A
    state->a = opcode[1];
    state->pc++;
    break;
  case 0x3F:
    state->flags.cy = 0;
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
  case 0x48: // MOV C,B
    state->c = state->b;
    break;
  case 0x49: // MOV C,C
    state->c = state->c;
    break;
  case 0x4A: // MOV C,D
    state->c = state->d;
    break;
  case 0x4B: // MOV C,E
    state->c = state->e;
    break;
  case 0x4C: // MOV C,H
    state->c = state->h;
    break;
  case 0x4D: // MOV C,L
    state->c = state->l;
    break;
  case 0x4E: // MOV C,M
    state->c = ReadFromHL(state);
    break;
  case 0x4F: // MOV C,A
    state->c = state->a;
    break;

  case 0x50: // MOV D,B
    state->d = state->b;
    break;
  case 0x51: // MOV D,C
    state->d = state->c;
    break;
  case 0x52: // MOV D,D
    state->d = state->d;
    break;
  case 0x53: // MOV D,E
    state->d = state->e;
    break;
  case 0x54: // MOV D,H
    state->d = state->h;
    break;
  case 0x55: // MOV D,L
    state->d = state->l;
    break;
  case 0x56: // MOV D,M
    state->d = ReadFromHL(state);
    break;
  case 0x57: // MOV D,A
    state->d = state->a;
    break;
  case 0x58: // MOV E,B
    state->e = state->b;
    break;
  case 0x59: // MOV E,C
    state->e = state->c;
    break;
  case 0x5A: // MOV E,D
    state->e = state->d;
    break;
  case 0x5B: // MOV E,E;
    state->e = state->e;
    break;
  case 0x5C: // MOV E,H
    state->e = state->h;
    break;
  case 0x5D: // MOV E,L
    state->e = state->l;
    break;
  case 0x5E: // MOV E,M
    state->e = ReadFromHL(state);
    break;
  case 0x5F: // MOV E,A
    state->e = state->a;
    break;
  case 0x60: // MOV H,B
    state->h = state->b;
    break;
  case 0x61: // MOV H,C
    state->h = state->c;
    break;
  case 0x62: // MOV H,D
    state->h = state->d;
    break;
  case 0x63: // MOV H,E
    state->h = state->e;
    break;
  case 0x64: // MOV H,H
    state->h = state->h;
    break;
  case 0x65: // MOV H,L
    state->h = state->l;
    break;
  case 0x66: // MOV H,M
    state->h = ReadFromHL(state);
    break;
  case 0x67: // MOV H,A
    state->h = state->a;
    break;
  case 0x68: // MOV L,B
    state->l = state->b;
    break;
  case 0x69: // MOV L,C
    state->l = state->c;
    break;
  case 0x6A: // MOV L,D
    state->l = state->d;
    break;
  case 0x6B: // MOV L,E
    state->l = state->e;
    break;
  case 0x6C: // MOV L,H
    state->l = state->h;
    break;
  case 0x6D: // MOV L,L
    state->l = state->l;
    break;
  case 0x6E: // MOV L,M
    state->l = ReadFromHL(state);
    break;
  case 0x6F: // MOV L,A
    state->l = state->a;
    break;

  case 0x70: // MOV M,B
    WriteToHL(state, state->b);
    break;
  case 0x71: // MOV M,C
    WriteToHL(state, state->c);
    break;
  case 0x72: // MOV M,D
    WriteToHL(state, state->d);
    break;
  case 0x73: // MOV M,E
    WriteToHL(state, state->e);
    break;
  case 0x74: // MOV M,H
    WriteToHL(state, state->h);
    break;
  case 0x75: // MOV M,L
    WriteToHL(state, state->l);
    break;
  case 0x76: // HLT
    break;
  case 0x77: // MOV M,A    -   (HL) <- A
    WriteToHL(state, state->a);
    break;
  case 0x78: // MOV A,B
    state->a = state->b;
    break;
  case 0x79: // MOV A,C
    state->a = state->c;
    break;
  case 0x7A: // MOV A,D
    state->a = state->d;
    break;
  case 0x7B: // MOV A,E
    state->a = state->e;
    break;
  case 0x7C: // MOV A,H
    state->a = state->h;
    break;
  case 0x7D: // MOV A,L
    state->a = state->l;
    break;
  case 0x7E: // MOV A,M
    state->a = ReadFromHL(state);
    break;
  case 0x7F: // MOV A,A
    state->a = state->a;
    break;

  case 0x80: // ADD B
  {
    uint16_t res = (uint16_t)state->a + (uint16_t)state->b;
    ArithFlagsA(state, res);
    state->a = (res & 0xff);
  } break;
  case 0x81: // ADD C
  {
    uint16_t res = (uint16_t)state->a + (uint16_t)state->c;
    ArithFlagsA(state, res);
    state->a = (res & 0xff);
  } break;
  case 0x82: // ADD D
  {
    uint16_t res = (uint16_t)state->a + (uint16_t)state->d;
    ArithFlagsA(state, res);
    state->a = (res & 0xff);
  } break;
  case 0x83: // ADD E
  {
    uint16_t res = (uint16_t)state->a + (uint16_t)state->e;
    ArithFlagsA(state, res);
    state->a = (res & 0xff);
  } break;
  case 0x84: // ADD H
  {
    uint16_t res = (uint16_t)state->a + (uint16_t)state->h;
    ArithFlagsA(state, res);
    state->a = (res & 0xff);
  } break;
  case 0x85: // ADD L
  {
    uint16_t res = (uint16_t)state->a + (uint16_t)state->l;
    ArithFlagsA(state, res);
    state->a = (res & 0xff);
  } break;
  case 0x86: // ADD M
  {
    uint16_t res = (uint16_t)state->a + (uint16_t)ReadFromHL(state);
    ArithFlagsA(state, res);
    state->a = (res & 0xff);
  } break;
  case 0x87: // ADD A
  {
    uint16_t res = (uint16_t)state->a + (uint16_t)state->a;
    ArithFlagsA(state, res);
    state->a = (res & 0xff);
  } break;
  case 0x88: // ADC B
  {
    uint16_t res = (uint16_t)state->a + (uint16_t)state->b + state->flags.cy;
    ArithFlagsA(state, res);
    state->a = (res & 0xff);
  } break;
  case 0x89: // ADC C
  {
    uint16_t res = (uint16_t)state->a + (uint16_t)state->c + state->flags.cy;
    ArithFlagsA(state, res);
    state->a = (res & 0xff);
  } break;
  case 0x8A: // ADC D
  {
    uint16_t res = (uint16_t)state->a + (uint16_t)state->d + state->flags.cy;
    ArithFlagsA(state, res);
    state->a = (res & 0xff);
  } break;
  case 0x8B: // ADC E
  {
    uint16_t res = (uint16_t)state->a + (uint16_t)state->e + state->flags.cy;
    ArithFlagsA(state, res);
    state->a = (res & 0xff);
  } break;
  case 0x8C: // ADC H
  {
    uint16_t res = (uint16_t)state->a + (uint16_t)state->h + state->flags.cy;
    ArithFlagsA(state, res);
    state->a = (res & 0xff);
  } break;
  case 0x8D: // ADC L
  {
    uint16_t res = (uint16_t)state->a + (uint16_t)state->l + state->flags.cy;
    ArithFlagsA(state, res);
    state->a = (res & 0xff);
  }; break;
  case 0x8E: // ADC M
  {
    uint16_t res =
        (uint16_t)state->a + (uint16_t)ReadFromHL(state) + state->flags.cy;
    ArithFlagsA(state, res);
    state->a = (res & 0xff);
  } break;
  case 0x8F: // ADC A
  {
    uint16_t res = (uint16_t)state->a + (uint16_t)state->a + state->flags.cy;
    ArithFlagsA(state, res);
    state->a = (res & 0xff);
  } break;

  case 0x90: // SUB B
  {
    uint16_t res = (uint16_t)state->a - (uint16_t)state->b;
    ArithFlagsA(state, res);
    state->a = (res & 0xff);
  } break;
  case 0x91: // SUB C
  {
    uint16_t res = (uint16_t)state->a - (uint16_t)state->c;
    ArithFlagsA(state, res);
    state->a = (res & 0xff);
  } break;
  case 0x92: // SUB D
  {
    uint16_t res = (uint16_t)state->a - (uint16_t)state->d;
    ArithFlagsA(state, res);
    state->a = (res & 0xff);
  } break;
  case 0x93: // SUB E
  {
    uint16_t res = (uint16_t)state->a - (uint16_t)state->e;
    ArithFlagsA(state, res);
    state->a = (res & 0xff);
  } break;
  case 0x94: // SUB H
  {
    uint16_t res = (uint16_t)state->a - (uint16_t)state->h;
    ArithFlagsA(state, res);
    state->a = (res & 0xff);
  } break;
  case 0x95: // SUB L
  {
    uint16_t res = (uint16_t)state->a - (uint16_t)state->l;
    ArithFlagsA(state, res);
    state->a = (res & 0xff);
  } break;
  case 0x96: // SUB M
  {
    uint16_t res = (uint16_t)state->a - (uint16_t)ReadFromHL(state);
    ArithFlagsA(state, res);
    state->a = (res & 0xff);
  } break;
  case 0x97: // SUB A
  {
    uint16_t res = (uint16_t)state->a - (uint16_t)state->a;
    ArithFlagsA(state, res);
    state->a = (res & 0xff);
  } break;
  case 0x98: // SBB B
  {
    uint16_t res = (uint16_t)state->a - (uint16_t)state->b - state->flags.cy;
    ArithFlagsA(state, res);
    state->a = (res & 0xff);
  } break;
  case 0x99: // SBB C
  {
    uint16_t res = (uint16_t)state->a - (uint16_t)state->c - state->flags.cy;
    ArithFlagsA(state, res);
    state->a = (res & 0xff);
  } break;
  case 0x9A: // SBB D
  {
    uint16_t res = (uint16_t)state->a - (uint16_t)state->d - state->flags.cy;
    ArithFlagsA(state, res);
    state->a = (res & 0xff);
  } break;
  case 0x9B: // SBB E
  {
    uint16_t res = (uint16_t)state->a - (uint16_t)state->e - state->flags.cy;
    ArithFlagsA(state, res);
    state->a = (res & 0xff);
  } break;
  case 0x9C: // SBB H
  {
    uint16_t res = (uint16_t)state->a - (uint16_t)state->h - state->flags.cy;
    ArithFlagsA(state, res);
    state->a = (res & 0xff);
  } break;
  case 0x9D: // SBB L
  {
    uint16_t res = (uint16_t)state->a - (uint16_t)state->l - state->flags.cy;
    ArithFlagsA(state, res);
    state->a = (res & 0xff);
  } break;
  case 0x9E: // SBB M
  {
    uint16_t res =
        (uint16_t)state->a - (uint16_t)ReadFromHL(state) - state->flags.cy;
    ArithFlagsA(state, res);
    state->a = (res & 0xff);
  } break;
  case 0x9F: // SBB A
  {
    uint16_t res = (uint16_t)state->a - (uint16_t)state->a - state->flags.cy;
    ArithFlagsA(state, res);
    state->a = (res & 0xff);
  } break;

  case 0xA0: // ANA B
  {
    state->a = (uint16_t)state->a & (uint16_t)state->b;
    LogicFlagsA(state);
  } break;
  case 0xA1: // ANA C
  {
    state->a = (uint16_t)state->a & (uint16_t)state->c;
    LogicFlagsA(state);
  } break;
  case 0xA2: // ANA D
  {
    state->a = (uint16_t)state->a & (uint16_t)state->d;
    LogicFlagsA(state);
  } break;
  case 0xA3: // ANA E
  {
    state->a = (uint16_t)state->a & (uint16_t)state->e;
    LogicFlagsA(state);
  } break;
  case 0xA4: // ANA H
  {
    state->a = (uint16_t)state->a & (uint16_t)state->h;
    LogicFlagsA(state);
  } break;
  case 0xA5: // ANA L
  {
    state->a = (uint16_t)state->a & (uint16_t)state->l;
    LogicFlagsA(state);
  } break;
  case 0xA6: // ANA M
  {
    state->a = (uint16_t)state->a & (uint16_t)ReadFromHL(state);
    LogicFlagsA(state);
  } break;
  case 0xA7: // ANA A - A <- A & A (Z, S, P, CY, AC)
  {
    state->a = state->a & state->a;
    LogicFlagsA(state);
  } break;
  case 0xA8: // XRA B
  {
    state->a = (uint16_t)state->a ^ (uint16_t)state->b;
    LogicFlagsA(state);
  } break;
  case 0xA9: // XRA C
  {
    state->a = (uint16_t)state->a ^ (uint16_t)state->c;
    LogicFlagsA(state);
  } break;
  case 0xAA: // XRA D
  {
    state->a = (uint16_t)state->a ^ (uint16_t)state->d;
    LogicFlagsA(state);
  } break;
  case 0xAB: // XRA E
  {
    state->a = (uint16_t)state->a ^ (uint16_t)state->e;
    LogicFlagsA(state);
  } break;
  case 0xAC: // XRA H
  {
    state->a = (uint16_t)state->a ^ (uint16_t)state->h;
    LogicFlagsA(state);
  } break;
  case 0xAD: // XRA L
  {
    state->a = (uint16_t)state->a ^ (uint16_t)state->l;
    LogicFlagsA(state);
  } break;
  case 0xAE: // XRA M
  {
    state->a = (uint16_t)state->a ^ (uint16_t)ReadFromHL(state);
    LogicFlagsA(state);
  } break;
  case 0xAF: // XRA A
  {
    state->a = state->a ^ state->a;
    LogicFlagsA(state);
  } break;

  case 0xB0: // ORA B
  {
    state->a = state->a | state->b;
    LogicFlagsA(state);
  } break;
  case 0xB1: // ORA C
  {
    state->a = state->a | state->c;
    LogicFlagsA(state);
  } break;
  case 0xB2: // ORA D
  {
    state->a = state->a | state->d;
    LogicFlagsA(state);
  } break;
  case 0xB3: // ORA E
  {
    state->a = state->a | state->e;
    LogicFlagsA(state);
  } break;
  case 0xB4: // ORA H
  {
    state->a = state->a | state->h;
    LogicFlagsA(state);
  } break;
  case 0xB5: // ORA L
  {
    state->a = state->a | state->l;
    LogicFlagsA(state);
  } break;
  case 0xB6: // ORA M
  {
    state->a = state->a | ReadFromHL(state);
    LogicFlagsA(state);
  } break;
  case 0xB7: // ORA A
  {
    state->a = state->a | state->a;
    LogicFlagsA(state);
  } break;
  case 0xB8: // CMP B
  {
    uint16_t res = (uint16_t)state->a - (uint16_t)state->b;
    ArithFlagsA(state, res);
  } break;
  case 0xB9: // CMP C
  {
    uint16_t res = (uint16_t)state->a - (uint16_t)state->c;
    ArithFlagsA(state, res);
  } break;
  case 0xBA: // CMP D
  {
    uint16_t res = (uint16_t)state->a - (uint16_t)state->d;
    ArithFlagsA(state, res);
  } break;
  case 0xBB: // CMP E
  {
    uint16_t res = (uint16_t)state->a - (uint16_t)state->e;
    ArithFlagsA(state, res);
  } break;
  case 0xBC: // CMP H
  {
    uint16_t res = (uint16_t)state->a - (uint16_t)state->h;
    ArithFlagsA(state, res);
  } break;
  case 0xBD: // CMP L
  {
    uint16_t res = (uint16_t)state->a - (uint16_t)state->l;
    ArithFlagsA(state, res);
  } break;
  case 0xBE: // CMP M
  {
    uint16_t res = (uint16_t)state->a - (uint16_t)ReadFromHL(state);
    ArithFlagsA(state, res);
  } break;
  case 0xBF: // CMP A
  {
    uint16_t res = (uint16_t)state->a - (uint16_t)state->a;
    ArithFlagsA(state, res);
  } break;

  case 0xC0: // RNZ
    if (state->flags.z == 0) {
      state->pc =
          state->memory[state->sp] | (state->memory[state->sp + 1] << 8);
      state->sp += 2;
    }
    break;
  case 0xC1: // POP B - C <- (sp); B <- (sp+1); sp <- sp+2
    Pop(state, &state->b, &state->c);
    break;
  case 0xC2: // JNZ
    if (state->flags.z == 0) {
      state->pc = (opcode[2] << 8) | opcode[1];
    } else {
      state->pc += 2;
    }
    break;
  case 0xC3: // JMP
    state->pc = (opcode[2] << 8) | opcode[1];
    break;
  case 0xC4: // CNZ
    if (state->flags.z == 0) {
      uint16_t ret = state->pc + 2;
      WriteMem(state, state->sp - 1, (ret >> 8) & 0xff);
      WriteMem(state, state->sp - 2, (ret & 0xff));
      state->sp = state->sp - 2;
      state->pc = (opcode[2] << 8) | opcode[1];
    } else {
      state->pc += 2;
    }
    break;
  case 0xC5: // PUSH B
  {
    Push(state, state->b, state->c);
  } break;
  case 0xC6: // ADI byte
  {
    uint16_t x = (uint16_t)state->a + (uint16_t)opcode[1];
    FlagsZSP(state, x & 0xff);
    state->flags.cy = (x > 0xff);
    state->a = x & 0xff;
    state->pc++;
  } break;
  case 0xC7: // RST 0
  {
    uint16_t ret = state->pc + 2;
    WriteMem(state, state->sp - 1, (ret >> 8) & 0xff);
    WriteMem(state, state->sp - 2, (ret & 0xff));
    state->sp = state->sp - 2;
    state->pc = 0x0000;
  } break;
  case 0xC8: // RZ
    if (state->flags.z) {
      state->pc =
          state->memory[state->sp] | (state->memory[state->sp + 1] << 8);
      state->sp += 2;
    }
    break;
  case 0xC9: // RET
    state->pc = state->memory[state->sp] | (state->memory[state->sp + 1] << 8);
    state->sp += 2;
    break;
  case 0xCA: // JZ
    if (state->flags.z) {
      state->pc = (opcode[2] << 8) | opcode[1];
    } else {
      state->pc += 2;
    }
    break;
  case 0xCB: // JMP shouldnt have to implement
    UnimplementedInstruction(state);
    break;
  case 0xCC: // CZ
    if (state->flags.z == 1) {
      uint16_t ret = state->pc + 2;
      WriteMem(state, state->sp - 1, (ret >> 8) & 0xff);
      WriteMem(state, state->sp - 2, (ret & 0xff));
      state->sp = state->sp - 2;
      state->pc = (opcode[2] << 8) | opcode[1];
    } else {
      state->pc += 2;
    }
    break;
  case 0xCD: // CALL
  {
    // #if DIAGNOSTICS
    //     if (5 == ((opcode[2] << 8) | opcode[1]))
    //         {
    //             if (state->c == 9)
    //             {
    //                 uint16_t offset = (state->d << 8) | (state->e);
    //                 char *str = &state->memory[offset + 3];
    //                 while (*str != '$')
    //                     printf("%c", *str++);
    //                 printf("\n");
    //             }
    //             else if (state->c == 2)
    //             {
    //                 printf("print char routine called\n");
    //             }
    //         }
    //     else if (0 == (opcode[2] << 8) | opcode[1])
    //     {
    //         // PrintLast1000();
    //         printf("I think this is an error!!\n");
    //         exit(0);
    //     }
    // #endif
    {
      uint16_t ret = state->pc + 2;
      WriteMem(state, state->sp - 1, (ret >> 8) & 0xff);
      WriteMem(state, state->sp - 2, (ret & 0xff));
      state->sp = state->sp - 2;
      state->pc = (opcode[2] << 8) | opcode[1];
    }
  } break;
  case 0xCE: // ACI
  {
    uint16_t x = state->a + opcode[1] + state->flags.cy;
    FlagsZSP(state, x & 0xff);
    state->flags.cy = (x > 0xff);
    state->a = x & 0xff;
    state->pc++;
  } break;
  case 0xCF: // RST 1
  {
    uint16_t ret = state->pc + 2;
    WriteMem(state, state->sp - 1, (ret >> 8) & 0xff);
    WriteMem(state, state->sp - 2, (ret & 0xff));
    state->sp = state->sp - 2;
    state->pc = 0x0008;
  } break;

  case 0xD0: // RNC
    if (state->flags.cy == 0) {
      state->pc =
          state->memory[state->sp] | (state->memory[state->sp + 1] << 8);
      state->sp += 2;
    }
    break;
  case 0xD1:
    Pop(state, &state->d, &state->e);
    break;
  case 0xD2: // JNC
    if (state->flags.cy == 0) {
      state->pc = (opcode[2] << 8) | opcode[1];
    } else {
      state->pc += 2;
    }
    break;
  case 0xD3: // OUT
    state->pc++;
    break;
  case 0xD4: // CNC
  {
    if (state->flags.cy == 0) {
      uint16_t ret = state->pc + 2;
      WriteMem(state, state->sp - 1, (ret >> 8) & 0xff);
      WriteMem(state, state->sp - 2, (ret & 0xff));
      state->sp = state->sp - 2;
      state->pc = (opcode[2] << 8) | opcode[1];
    } else {
      state->pc += 2;
    }
  } break;
  case 0xD5: // PUSH D
    Push(state, state->d, state->e);
    break;
  case 0xD6: // SUI
  {
    uint8_t x = state->a - opcode[1];
    FlagsZSP(state, x & 0xff);
    state->flags.cy = (state->a < opcode[1]);
    state->a = x;
    state->pc++;
  } break;
  case 0xD7: // RST 2
  {
    uint16_t ret = state->pc + 2;
    WriteMem(state, state->sp - 1, (ret >> 8) & 0xff);
    WriteMem(state, state->sp - 2, (ret & 0xff));
    state->sp = state->sp - 2;
    state->pc = 0x10;
  } break;
  case 0xD8: // RC
    if (state->flags.cy != 0) {
      state->pc =
          state->memory[state->sp] | (state->memory[state->sp + 1] << 8);
      state->sp += 2;
    }
    break;
  case 0xD9: // RET shouldnt have to implement
    UnimplementedInstruction(state);
    break;
  case 0xDA: // JC - if cy, PC<-adr
    if (state->flags.cy != 0) {
      state->pc = (opcode[2] << 8) | opcode[1];
    } else {
      state->pc += 2;
    }
    break;
  case 0xDB:
    state->pc++;
    break;
  case 0xDC: // CC
    if (state->flags.cy != 0) {
      uint16_t ret = state->pc + 2;
      WriteMem(state, state->sp - 1, (ret >> 8) & 0xff);
      WriteMem(state, state->sp - 2, (ret & 0xff));
      state->sp = state->sp - 2;
      state->pc = (opcode[2] << 8) | opcode[1];
    } else {
      state->pc += 2;
    }
    break;
  case 0xDD: // CALL shouldnt have to implement
    UnimplementedInstruction(state);
    break;
  case 0xDE: // SBI byte
  {
    uint16_t x = state->a - opcode[1] - state->flags.cy;
    FlagsZSP(state, x & 0xff);
    state->flags.cy = (x > 0xff);
    state->a = x & 0xff;
    state->pc++;
  } break;
  case 0xDF: // RST 3
  {
    uint16_t ret = state->pc + 2;
    WriteMem(state, state->sp - 1, (ret >> 8) & 0xff);
    WriteMem(state, state->sp - 2, (ret & 0xff));
    state->sp = state->sp - 2;
    state->pc = 0x18;
  } break;

  case 0xE0: // RPO
    if (state->flags.p == 0) {
      state->pc =
          state->memory[state->sp] | (state->memory[state->sp + 1] << 8);
      state->sp += 2;
    }
    break;
  case 0xE1: // POP H - L <- (sp); H <- (sp+1); sp <- sp+2
  {
    Pop(state, &state->h, &state->l);
  } break;
  case 0xE2: // JPO
    if (state->flags.p == 0) {
      state->pc = (opcode[2] << 8) | opcode[1];
    } else {
      state->pc += 2;
    }
    break;
  case 0xE3: // XTHL
  {
    uint8_t h = state->h;
    uint8_t l = state->l;
    state->l = state->memory[state->sp];
    state->h = state->memory[state->sp + 1];
    WriteMem(state, state->sp, l);
    WriteMem(state, state->sp + 1, h);
  } break;
  case 0xE4: // CPO
    if (state->flags.p == 0) {
      uint16_t ret = state->pc + 2;
      WriteMem(state, state->sp - 1, (ret >> 8) & 0xff);
      WriteMem(state, state->sp - 2, (ret & 0xff));
      state->sp = state->sp - 2;
      state->pc = (opcode[2] << 8) | opcode[1];
    } else {
      state->pc += 2;
    }
    break;
  case 0xE5: // PUSH H
    Push(state, state->h, state->l);
    break;
  case 0xE6: // ANI A <- A & data
  {
    state->a = state->a & opcode[1];
    LogicFlagsA(state);
    state->pc++;
  } break;
  case 0xE7: // RST 4
  {
    uint16_t ret = state->pc + 2;
    WriteMem(state, state->sp - 1, (ret >> 8) & 0xff);
    WriteMem(state, state->sp - 2, (ret & 0xff));
    state->sp = state->sp - 2;
    state->pc = 0x20;
  } break;
  case 0xE8: // RPE
    if (state->flags.p != 0) {
      state->pc =
          state->memory[state->sp] | (state->memory[state->sp + 1] << 8);
      state->sp += 2;
    }
    break;
  case 0xE9: // PCHL
    state->pc = (state->h << 8) | state->l;
    break;
  case 0xEA: // JPE
    if (state->flags.p != 0) {
      state->pc = (opcode[2] << 8) | opcode[1];
    } else {
      state->pc += 2;
    }
    break;
  case 0xEB: // XCHG -     H <-> D; L <-> E
  {
    uint8_t save1 = state->d;
    uint8_t save2 = state->e;
    state->d = state->h;
    state->e = state->l;
    state->h = save1;
    state->l = save2;
  } break;
  case 0xEC: // CPE
    if (state->flags.p != 0) {
      uint16_t ret = state->pc + 2;
      WriteMem(state, state->sp - 1, (ret >> 8) & 0xff);
      WriteMem(state, state->sp - 2, (ret & 0xff));
      state->sp = state->sp - 2;
      state->pc = (opcode[2] << 8) | opcode[1];
    } else {
      state->pc += 2;
    }
    break;
  case 0xED: // CALL // shouldnt have to implement
    UnimplementedInstruction(state);
    break;
  case 0xEE: // XRI byte
  {
    uint8_t x = state->a ^ opcode[1];
    FlagsZSP(state, x);
    state->flags.cy = 0;
    state->a = x;
    state->pc++;
  } break;
  case 0xEF: // RST 5
  {
    uint16_t ret = state->pc + 2;
    WriteMem(state, state->sp - 1, (ret >> 8) & 0xff);
    WriteMem(state, state->sp - 2, (ret & 0xff));
    state->sp = state->sp - 2;
    state->pc = 0x28;
  } break;

  case 0xF0: // RP
    if (state->flags.s == 0) {
      state->pc =
          state->memory[state->sp] | (state->memory[state->sp + 1] << 8);
      state->sp += 2;
    }
    break;
  case 0xF1: // POP PSW - flags <- (sp); A <- (sp+1); sp <- sp+2
    Pop(state, &state->a, (unsigned char *)&state->flags);
    break;
  case 0xF2: // JP
    if (state->flags.s == 0) {
      state->pc = (opcode[2] << 8) | opcode[1];
    } else {
      state->pc += 2;
    }
    break;
  case 0xF3: // DI
    state->int_enable = 0;
    break;
  case 0xF4: // CP
  {
    if (state->flags.s == 0) {
      uint16_t ret = state->pc + 2;
      WriteMem(state, state->sp - 1, (ret >> 8) & 0xff);
      WriteMem(state, state->sp - 2, (ret & 0xff));
      state->sp = state->sp - 2;
      state->pc = (opcode[2] << 8) | opcode[1];
    } else {
      state->pc += 2;
    }
  } break;
  case 0xF5: // PUSH PSW -     (sp-2)<-flags; (sp-1)<-A; sp <- sp - 2
  {
    Push(state, state->a, *(unsigned char *)&state->flags);
  } break;
  case 0xF6: // ORI
  {
    uint8_t x = state->a | opcode[1];
    FlagsZSP(state, x);
    state->flags.cy = 0;
    state->a = x;
    state->pc++;
  } break;
  case 0xF7: // RST 6
  {
    uint16_t ret = state->pc + 2;
    WriteMem(state, state->sp - 1, (ret >> 8) & 0xff);
    WriteMem(state, state->sp - 2, (ret & 0xff));
    state->sp = state->sp - 2;
    state->pc = 0x30;
  } break;
  case 0xF8: // RM
    if (state->flags.s != 0) {
      state->pc =
          state->memory[state->sp] | (state->memory[state->sp + 1] << 8);
      state->sp += 2;
    }
    break;
  case 0xF9: // SPHL
    state->sp = state->l | (state->h << 8);
    break;
  case 0xFA: // JM
    if (state->flags.s != 0) {
      state->pc = (opcode[2] << 8) | opcode[1];
    } else {
      state->pc += 2;
    }
    break;
  case 0xFB: // EI
    state->int_enable = 1;
    break;
  case 0xFC: // CM - if (M) call adr
    if (state->flags.s != 0) {
      uint16_t ret = state->pc + 2;
      WriteMem(state, state->sp - 1, (ret >> 8) & 0xff);
      WriteMem(state, state->sp - 2, (ret & 0xff));
      state->sp = state->sp - 2;
      state->pc = (opcode[2] << 8) | opcode[1];
    } else {
      state->pc += 2;
    }
    break;
  case 0xFD: // CALL shouldnt have to implement
    UnimplementedInstruction(state);
    break;
  case 0xFE: // CPI
  {
    uint8_t x = state->a - opcode[1];
    FlagsZSP(state, x);
    state->flags.cy = (state->a < opcode[1]);
    state->pc++;
  } break;
  case 0xFF: // RST 7
  {
    uint16_t ret = state->pc + 2;
    WriteMem(state, state->sp - 1, (ret >> 8) & 0xff);
    WriteMem(state, state->sp - 2, (ret & 0xff));
    state->sp = state->sp - 2;
    state->pc = 0x38;
  } break;
  }
#if PRINTOPS
  printf("\tC=%d,P=%d,S=%d,Z=%d,AC=%d\n", state->flags.cy, state->flags.p,
         state->flags.s, state->flags.z, state->flags.ac);
  printf("\tA $%02x BC $%02x%02x DE $%02x%02x HL $%02x%02x SP %04x PC %04x\n",
         state->a, state->b, state->c, state->d, state->e, state->h, state->l,
         state->sp, state->pc);
#endif
#if DIAGNOSTICS
  Disassemble8080(state->memory, state->pc);
  printf("\tC=%d,P=%d,S=%d,Z=%d,AC=%d\n", state->flags.cy, state->flags.p,
         state->flags.s, state->flags.z, state->flags.ac);
  printf("\tA $%02x BC $%02x%02x DE $%02x%02x HL $%02x%02x SP %04x PC %04x\n",
         state->a, state->b, state->c, state->d, state->e, state->h, state->l,
         state->sp, state->pc);
  printf("\n");
#endif
  return cycles8080[*opcode];
}

void GenerateInterrupt(State8080 *state, int interrupt_num) {
  // perform "PUSH PC"
  Push(state, (state->pc & 0xFF00) >> 8, (state->pc & 0xff));
  // set PC to low memory vector
  state->pc = 8 * interrupt_num;
  // "DI"
  state->int_enable = 0;
}
