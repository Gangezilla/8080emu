#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #include <stdint.h>

// uint8 is an unsigned char, can hold values  0 - 255.
typedef struct Flags
{
  uint8_t z : 1;   // zero flag
  uint8_t s : 1;   // sign flag
  uint8_t p : 1;   // parity flag
  uint8_t cy : 1;  // carry flag
  uint8_t ac : 1;  // auxilary carry flag
  uint8_t pad : 3; // parity flag
} Flags;

typedef struct State8080
{
  uint8_t a;
  uint8_t b;
  uint8_t c;
  uint8_t d;
  uint8_t e;
  uint8_t h;
  uint8_t l;
  uint16_t sp; // stack pointer
  uint16_t pc;
  uint8_t *memory;
  struct Flags flags;
  uint8_t int_enable;
} State8080;

int Disassemble8080(unsigned char *codebuffer, int pc)
{
  unsigned char *code = &codebuffer[pc];
  int opbytes = 1;
  printf("%07x ", pc);
  switch (*code)
  {
  case 0x00:
    printf("NOP");
    break;
  case 0x01:
    printf("LXI    B,#$%02x%02x", code[2], code[1]);
    opbytes = 3;
    break;
  case 0x02:
    printf("STAX   B");
    break;
  case 0x03:
    printf("INX    B");
    break;
  case 0x04:
    printf("INR    B");
    break;
  case 0x05:
    printf("DCR    B");
    break;
  case 0x06:
    printf("MVI    B,#$%02x", code[1]);
    opbytes = 2;
    break;
  case 0x07:
    printf("RLC");
    break;
  case 0x08:
    printf("NOP");
    break;
  case 0x09:
    printf("DAD    B");
    break;
  case 0x0a:
    printf("LDAX   B");
    break;
  case 0x0b:
    printf("DCX    B");
    break;
  case 0x0c:
    printf("INR    C");
    break;
  case 0x0d:
    printf("DCR    C");
    break;
  case 0x0e:
    printf("MVI    C,#$%02x", code[1]);
    opbytes = 2;
    break;
  case 0x0f:
    printf("RRC");
    break;

  case 0x10:
    printf("NOP");
    break;
  case 0x11:
    printf("LXI    D,#$%02x%02x", code[2], code[1]);
    opbytes = 3;
    break;
  case 0x12:
    printf("STAX   D");
    break;
  case 0x13:
    printf("INX    D");
    break;
  case 0x14:
    printf("INR    D");
    break;
  case 0x15:
    printf("DCR    D");
    break;
  case 0x16:
    printf("MVI    D,#$%02x", code[1]);
    opbytes = 2;
    break;
  case 0x17:
    printf("RAL");
    break;
  case 0x18:
    printf("NOP");
    break;
  case 0x19:
    printf("DAD    D");
    break;
  case 0x1a:
    printf("LDAX   D");
    break;
  case 0x1b:
    printf("DCX    D");
    break;
  case 0x1c:
    printf("INR    E");
    break;
  case 0x1d:
    printf("DCR    E");
    break;
  case 0x1e:
    printf("MVI    E,#$%02x", code[1]);
    opbytes = 2;
    break;
  case 0x1f:
    printf("RAR");
    break;

  case 0x20:
    printf("NOP");
    break;
  case 0x21:
    printf("LXI    H,#$%02x%02x", code[2], code[1]);
    opbytes = 3;
    break;
  case 0x22:
    printf("SHLD   $%02x%02x", code[2], code[1]);
    opbytes = 3;
    break;
  case 0x23:
    printf("INX    H");
    break;
  case 0x24:
    printf("INR    H");
    break;
  case 0x25:
    printf("DCR    H");
    break;
  case 0x26:
    printf("MVI    H,#$%02x", code[1]);
    opbytes = 2;
    break;
  case 0x27:
    printf("DAA");
    break;
  case 0x28:
    printf("NOP");
    break;
  case 0x29:
    printf("DAD    H");
    break;
  case 0x2a:
    printf("LHLD   $%02x%02x", code[2], code[1]);
    opbytes = 3;
    break;
  case 0x2b:
    printf("DCX    H");
    break;
  case 0x2c:
    printf("INR    L");
    break;
  case 0x2d:
    printf("DCR    L");
    break;
  case 0x2e:
    printf("MVI    L,#$%02x", code[1]);
    opbytes = 2;
    break;
  case 0x2f:
    printf("CMA");
    break;

  case 0x30:
    printf("NOP");
    break;
  case 0x31:
    printf("LXI    SP,#$%02x%02x", code[2], code[1]);
    opbytes = 3;
    break;
  case 0x32:
    printf("STA    $%02x%02x", code[2], code[1]);
    opbytes = 3;
    break;
  case 0x33:
    printf("INX    SP");
    break;
  case 0x34:
    printf("INR    M");
    break;
  case 0x35:
    printf("DCR    M");
    break;
  case 0x36:
    printf("MVI    M,#$%02x", code[1]);
    opbytes = 2;
    break;
  case 0x37:
    printf("STC");
    break;
  case 0x38:
    printf("NOP");
    break;
  case 0x39:
    printf("DAD    SP");
    break;
  case 0x3a:
    printf("LDA    $%02x%02x", code[2], code[1]);
    opbytes = 3;
    break;
  case 0x3b:
    printf("DCX    SP");
    break;
  case 0x3c:
    printf("INR    A");
    break;
  case 0x3d:
    printf("DCR    A");
    break;
  case 0x3e:
    printf("MVI    A,#$%02x", code[1]);
    opbytes = 2;
    break;
  case 0x3f:
    printf("CMC");
    break;

  case 0x40:
    printf("MOV    B,B");
    break;
  case 0x41:
    printf("MOV    B,C");
    break;
  case 0x42:
    printf("MOV    B,D");
    break;
  case 0x43:
    printf("MOV    B,E");
    break;
  case 0x44:
    printf("MOV    B,H");
    break;
  case 0x45:
    printf("MOV    B,L");
    break;
  case 0x46:
    printf("MOV    B,M");
    break;
  case 0x47:
    printf("MOV    B,A");
    break;
  case 0x48:
    printf("MOV    C,B");
    break;
  case 0x49:
    printf("MOV    C,C");
    break;
  case 0x4a:
    printf("MOV    C,D");
    break;
  case 0x4b:
    printf("MOV    C,E");
    break;
  case 0x4c:
    printf("MOV    C,H");
    break;
  case 0x4d:
    printf("MOV    C,L");
    break;
  case 0x4e:
    printf("MOV    C,M");
    break;
  case 0x4f:
    printf("MOV    C,A");
    break;

  case 0x50:
    printf("MOV    D,B");
    break;
  case 0x51:
    printf("MOV    D,C");
    break;
  case 0x52:
    printf("MOV    D,D");
    break;
  case 0x53:
    printf("MOV    D.E");
    break;
  case 0x54:
    printf("MOV    D,H");
    break;
  case 0x55:
    printf("MOV    D,L");
    break;
  case 0x56:
    printf("MOV    D,M");
    break;
  case 0x57:
    printf("MOV    D,A");
    break;
  case 0x58:
    printf("MOV    E,B");
    break;
  case 0x59:
    printf("MOV    E,C");
    break;
  case 0x5a:
    printf("MOV    E,D");
    break;
  case 0x5b:
    printf("MOV    E,E");
    break;
  case 0x5c:
    printf("MOV    E,H");
    break;
  case 0x5d:
    printf("MOV    E,L");
    break;
  case 0x5e:
    printf("MOV    E,M");
    break;
  case 0x5f:
    printf("MOV    E,A");
    break;

  case 0x60:
    printf("MOV    H,B");
    break;
  case 0x61:
    printf("MOV    H,C");
    break;
  case 0x62:
    printf("MOV    H,D");
    break;
  case 0x63:
    printf("MOV    H.E");
    break;
  case 0x64:
    printf("MOV    H,H");
    break;
  case 0x65:
    printf("MOV    H,L");
    break;
  case 0x66:
    printf("MOV    H,M");
    break;
  case 0x67:
    printf("MOV    H,A");
    break;
  case 0x68:
    printf("MOV    L,B");
    break;
  case 0x69:
    printf("MOV    L,C");
    break;
  case 0x6a:
    printf("MOV    L,D");
    break;
  case 0x6b:
    printf("MOV    L,E");
    break;
  case 0x6c:
    printf("MOV    L,H");
    break;
  case 0x6d:
    printf("MOV    L,L");
    break;
  case 0x6e:
    printf("MOV    L,M");
    break;
  case 0x6f:
    printf("MOV    L,A");
    break;

  case 0x70:
    printf("MOV    M,B");
    break;
  case 0x71:
    printf("MOV    M,C");
    break;
  case 0x72:
    printf("MOV    M,D");
    break;
  case 0x73:
    printf("MOV    M.E");
    break;
  case 0x74:
    printf("MOV    M,H");
    break;
  case 0x75:
    printf("MOV    M,L");
    break;
  case 0x76:
    printf("HLT");
    break;
  case 0x77:
    printf("MOV    M,A");
    break;
  case 0x78:
    printf("MOV    A,B");
    break;
  case 0x79:
    printf("MOV    A,C");
    break;
  case 0x7a:
    printf("MOV    A,D");
    break;
  case 0x7b:
    printf("MOV    A,E");
    break;
  case 0x7c:
    printf("MOV    A,H");
    break;
  case 0x7d:
    printf("MOV    A,L");
    break;
  case 0x7e:
    printf("MOV    A,M");
    break;
  case 0x7f:
    printf("MOV    A,A");
    break;

  case 0x80:
    printf("ADD    B");
    break;
  case 0x81:
    printf("ADD    C");
    break;
  case 0x82:
    printf("ADD    D");
    break;
  case 0x83:
    printf("ADD    E");
    break;
  case 0x84:
    printf("ADD    H");
    break;
  case 0x85:
    printf("ADD    L");
    break;
  case 0x86:
    printf("ADD    M");
    break;
  case 0x87:
    printf("ADD    A");
    break;
  case 0x88:
    printf("ADC    B");
    break;
  case 0x89:
    printf("ADC    C");
    break;
  case 0x8a:
    printf("ADC    D");
    break;
  case 0x8b:
    printf("ADC    E");
    break;
  case 0x8c:
    printf("ADC    H");
    break;
  case 0x8d:
    printf("ADC    L");
    break;
  case 0x8e:
    printf("ADC    M");
    break;
  case 0x8f:
    printf("ADC    A");
    break;

  case 0x90:
    printf("SUB    B");
    break;
  case 0x91:
    printf("SUB    C");
    break;
  case 0x92:
    printf("SUB    D");
    break;
  case 0x93:
    printf("SUB    E");
    break;
  case 0x94:
    printf("SUB    H");
    break;
  case 0x95:
    printf("SUB    L");
    break;
  case 0x96:
    printf("SUB    M");
    break;
  case 0x97:
    printf("SUB    A");
    break;
  case 0x98:
    printf("SBB    B");
    break;
  case 0x99:
    printf("SBB    C");
    break;
  case 0x9a:
    printf("SBB    D");
    break;
  case 0x9b:
    printf("SBB    E");
    break;
  case 0x9c:
    printf("SBB    H");
    break;
  case 0x9d:
    printf("SBB    L");
    break;
  case 0x9e:
    printf("SBB    M");
    break;
  case 0x9f:
    printf("SBB    A");
    break;

  case 0xa0:
    printf("ANA    B");
    break;
  case 0xa1:
    printf("ANA    C");
    break;
  case 0xa2:
    printf("ANA    D");
    break;
  case 0xa3:
    printf("ANA    E");
    break;
  case 0xa4:
    printf("ANA    H");
    break;
  case 0xa5:
    printf("ANA    L");
    break;
  case 0xa6:
    printf("ANA    M");
    break;
  case 0xa7:
    printf("ANA    A");
    break;
  case 0xa8:
    printf("XRA    B");
    break;
  case 0xa9:
    printf("XRA    C");
    break;
  case 0xaa:
    printf("XRA    D");
    break;
  case 0xab:
    printf("XRA    E");
    break;
  case 0xac:
    printf("XRA    H");
    break;
  case 0xad:
    printf("XRA    L");
    break;
  case 0xae:
    printf("XRA    M");
    break;
  case 0xaf:
    printf("XRA    A");
    break;

  case 0xb0:
    printf("ORA    B");
    break;
  case 0xb1:
    printf("ORA    C");
    break;
  case 0xb2:
    printf("ORA    D");
    break;
  case 0xb3:
    printf("ORA    E");
    break;
  case 0xb4:
    printf("ORA    H");
    break;
  case 0xb5:
    printf("ORA    L");
    break;
  case 0xb6:
    printf("ORA    M");
    break;
  case 0xb7:
    printf("ORA    A");
    break;
  case 0xb8:
    printf("CMP    B");
    break;
  case 0xb9:
    printf("CMP    C");
    break;
  case 0xba:
    printf("CMP    D");
    break;
  case 0xbb:
    printf("CMP    E");
    break;
  case 0xbc:
    printf("CMP    H");
    break;
  case 0xbd:
    printf("CMP    L");
    break;
  case 0xbe:
    printf("CMP    M");
    break;
  case 0xbf:
    printf("CMP    A");
    break;

  case 0xc0:
    printf("RNZ");
    break;
  case 0xc1:
    printf("POP    B");
    break;
  case 0xc2:
    printf("JNZ    $%02x%02x", code[2], code[1]);
    opbytes = 3;
    break;
  case 0xc3:
    printf("JMP    $%02x%02x", code[2], code[1]);
    opbytes = 3;
    break;
  case 0xc4:
    printf("CNZ    $%02x%02x", code[2], code[1]);
    opbytes = 3;
    break;
  case 0xc5:
    printf("PUSH   B");
    break;
  case 0xc6:
    printf("ADI    #$%02x", code[1]);
    opbytes = 2;
    break;
  case 0xc7:
    printf("RST    0");
    break;
  case 0xc8:
    printf("RZ");
    break;
  case 0xc9:
    printf("RET");
    break;
  case 0xca:
    printf("JZ     $%02x%02x", code[2], code[1]);
    opbytes = 3;
    break;
  case 0xcb:
    printf("JMP    $%02x%02x", code[2], code[1]);
    opbytes = 3;
    break;
  case 0xcc:
    printf("CZ     $%02x%02x", code[2], code[1]);
    opbytes = 3;
    break;
  case 0xcd:
    printf("CALL   $%02x%02x", code[2], code[1]);
    opbytes = 3;
    break;
  case 0xce:
    printf("ACI    #$%02x", code[1]);
    opbytes = 2;
    break;
  case 0xcf:
    printf("RST    1");
    break;

  case 0xd0:
    printf("RNC");
    break;
  case 0xd1:
    printf("POP    D");
    break;
  case 0xd2:
    printf("JNC    $%02x%02x", code[2], code[1]);
    opbytes = 3;
    break;
  case 0xd3:
    printf("OUT    #$%02x", code[1]);
    opbytes = 2;
    break;
  case 0xd4:
    printf("CNC    $%02x%02x", code[2], code[1]);
    opbytes = 3;
    break;
  case 0xd5:
    printf("PUSH   D");
    break;
  case 0xd6:
    printf("SUI    #$%02x", code[1]);
    opbytes = 2;
    break;
  case 0xd7:
    printf("RST    2");
    break;
  case 0xd8:
    printf("RC");
    break;
  case 0xd9:
    printf("RET");
    break;
  case 0xda:
    printf("JC     $%02x%02x", code[2], code[1]);
    opbytes = 3;
    break;
  case 0xdb:
    printf("IN     #$%02x", code[1]);
    opbytes = 2;
    break;
  case 0xdc:
    printf("CC     $%02x%02x", code[2], code[1]);
    opbytes = 3;
    break;
  case 0xdd:
    printf("CALL   $%02x%02x", code[2], code[1]);
    opbytes = 3;
    break;
  case 0xde:
    printf("SBI    #$%02x", code[1]);
    opbytes = 2;
    break;
  case 0xdf:
    printf("RST    3");
    break;

  case 0xe0:
    printf("RPO");
    break;
  case 0xe1:
    printf("POP    H");
    break;
  case 0xe2:
    printf("JPO    $%02x%02x", code[2], code[1]);
    opbytes = 3;
    break;
  case 0xe3:
    printf("XTHL");
    break;
  case 0xe4:
    printf("CPO    $%02x%02x", code[2], code[1]);
    opbytes = 3;
    break;
  case 0xe5:
    printf("PUSH   H");
    break;
  case 0xe6:
    printf("ANI    #$%02x", code[1]);
    opbytes = 2;
    break;
  case 0xe7:
    printf("RST    4");
    break;
  case 0xe8:
    printf("RPE");
    break;
  case 0xe9:
    printf("PCHL");
    break;
  case 0xea:
    printf("JPE    $%02x%02x", code[2], code[1]);
    opbytes = 3;
    break;
  case 0xeb:
    printf("XCHG");
    break;
  case 0xec:
    printf("CPE     $%02x%02x", code[2], code[1]);
    opbytes = 3;
    break;
  case 0xed:
    printf("CALL   $%02x%02x", code[2], code[1]);
    opbytes = 3;
    break;
  case 0xee:
    printf("XRI    #$%02x", code[1]);
    opbytes = 2;
    break;
  case 0xef:
    printf("RST    5");
    break;

  case 0xf0:
    printf("RP");
    break;
  case 0xf1:
    printf("POP    PSW");
    break;
  case 0xf2:
    printf("JP     $%02x%02x", code[2], code[1]);
    opbytes = 3;
    break;
  case 0xf3:
    printf("DI");
    break;
  case 0xf4:
    printf("CP     $%02x%02x", code[2], code[1]);
    opbytes = 3;
    break;
  case 0xf5:
    printf("PUSH   PSW");
    break;
  case 0xf6:
    printf("ORI    #$%02x", code[1]);
    opbytes = 2;
    break;
  case 0xf7:
    printf("RST    6");
    break;
  case 0xf8:
    printf("RM");
    break;
  case 0xf9:
    printf("SPHL");
    break;
  case 0xfa:
    printf("JM     $%02x%02x", code[2], code[1]);
    opbytes = 3;
    break;
  case 0xfb:
    printf("EI");
    break;
  case 0xfc:
    printf("CM     $%02x%02x", code[2], code[1]);
    opbytes = 3;
    break;
  case 0xfd:
    printf("CALL   $%02x%02x", code[2], code[1]);
    opbytes = 3;
    break;
  case 0xfe:
    printf("CPI    #$%02x", code[1]);
    opbytes = 2;
    break;
  case 0xff:
    printf("RST    7");
    break;
  }
  printf("\n");
  return opbytes;
}

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

void MachineOUT(State8080 *state, uint8_t port)
// has 2,3,4,5,6
{
  switch (port)
  {
  case 2:
    // shift register result offset (bits 0,1,2)
    break;
  case 3:
    // play a sound of some kind
    break;
  case 4:
    // fill shift register
    break;
  case 5:
    // play a different sound
    break;
  case 6:
    // debug port?
    break;
  }
}

uint8_t MachineIN(State8080 *state, uint8_t port)
// has 1,2,3
{
  {
    switch (port)
    {
    case 1:

      break;
    case 2:

      break;
    case 3:

      break;
    }
    return 1;
  }
}

void UnimplementedInstruction(State8080 *state)
{
  state->pc--; // pc incremented at start of emulate8080, undo that.
  printf("Error: Unimplemented instruction\n");
  // Disassemble8080(state->memory, state->pc);
  exit(1);
}

int Emulate8080(State8080 *state)
{
  // * turns a pointer into a value
  // & turns a value into a pointer
  // -> is used to access members of a struct when theyre a pointer.
  int cycles = 4;
  unsigned char *opcode = &state->memory[state->pc];
  Disassemble8080(state->memory, state->pc);
  state->pc += 1;

  switch (*opcode)
  {
  case 0x00: // NOP
  case 0x08:
  case 0x10:
  case 0x18:
  // case 0x20:
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
  case 0x03:
    UnimplementedInstruction(state);
    break;
  case 0x04:
    UnimplementedInstruction(state);
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
  case 0x07:
    UnimplementedInstruction(state);
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
  case 0x0A:
    UnimplementedInstruction(state);
    break;
  case 0x0B:
    UnimplementedInstruction(state);
    break;
  case 0x0C:
    UnimplementedInstruction(state);
    break;
  case 0x0D: // DCR C
  {
    uint8_t res = state->c - 1;
    state->flags.z = (res == 0);
    state->flags.s = (0x80 == (res & 0x80));
    state->flags.p = parity(res, 8);
    state->c = res;
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
  case 0x12: // STAX D - 	(DE) <- A
  {
    uint16_t offset = (state->d << 8) | state->e;
    state->memory[offset] = state->a;
    state->pc += 2;
  }
  break;
  case 0x13: // INX D  -  (DE) <- DE + 1
    state->e++;
    if (state->e == 0)
    {
      state->d++;
    }
    break;
  case 0x14:
    UnimplementedInstruction(state);
    break;
  case 0x15:
    UnimplementedInstruction(state);
    break;
  case 0x16:
    UnimplementedInstruction(state);
    break;
  case 0x17:
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
  }
  break;
  case 0x1A: // LDAX D   -   A <- (DE)
  {
    uint16_t offset = (state->d << 8) | state->e; // grabs the memory location from registers d and e.
    state->a = state->memory[offset];             // stores the content of that location in register a.
  }
  break;
  case 0x1B:
    UnimplementedInstruction(state);
    break;
  case 0x1C:
    UnimplementedInstruction(state);
    break;
  case 0x1D:
    UnimplementedInstruction(state);
    break;
  case 0x1E:
    UnimplementedInstruction(state);
    break;
  case 0x1F:
    UnimplementedInstruction(state);
    break;

  case 0x20:
    UnimplementedInstruction(state);
    break;
  case 0x21: // LXI H
    state->l = opcode[1];
    state->h = opcode[2];
    state->pc += 2;
    break;
  case 0x22:
    UnimplementedInstruction(state);
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
  case 0x24:
    UnimplementedInstruction(state);
    break;
  case 0x25:
    UnimplementedInstruction(state);
    break;
  case 0x26: // MVI H
    state->h = opcode[1];
    state->pc++;
    break;
  case 0x27:
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
  }
  break;
  case 0x2A:
    UnimplementedInstruction(state);
    break;
  case 0x2B:
    UnimplementedInstruction(state);
    break;
  case 0x2C:
    UnimplementedInstruction(state);
    break;
  case 0x2D:
    UnimplementedInstruction(state);
    break;
  case 0x2E: // MVI L - L <- byte 2
  {
    state->l = opcode[1];
    // state->pc++;
    // NOT SURE
    state->sp++;
  }
  break;
  case 0x2F:
    UnimplementedInstruction(state);
    break;
  case 0x31: // LXI
    state->sp = (opcode[2] << 8) | opcode[1];
    state->pc += 2;
    break;
  case 0x32: // STA - 	(adr) <- A (store A to memory)
  {
    uint16_t offset = (opcode[2] << 8 | opcode[1]);
    state->memory[offset] = state->a;
    state->pc += 2;
  }
  break;
    UnimplementedInstruction(state);
    break;
  case 0x33:
    UnimplementedInstruction(state);
    break;
  case 0x34:
    UnimplementedInstruction(state);
    break;
  case 0x35:
    UnimplementedInstruction(state);
    break;
  case 0x36: // MVI M
  {
    uint16_t offset = (state->h << 8) | state->l;
    state->memory[offset] = opcode[1];
    state->pc++;
    break;
  }
  break;
  case 0x37:
    UnimplementedInstruction(state);
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
    state->a = state->a + 1;
    state->flags.z = ((state->a & 0xff) == 0);
    state->flags.s = (state->a & 0x80);
    state->flags.p = parity(state->a, 8);
  }
  break;
  case 0x3D: // DCR A - A <- A - 1 - NOT SURE
  {
    uint8_t res = state->a - 1;
    state->flags.z = (res == 0);
    state->flags.s = (0x80 == (res & 0x80));
    state->flags.p = parity(res, 8);
    state->a = res;
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
    UnimplementedInstruction(state);
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
  case 0x4F:
    UnimplementedInstruction(state);
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
    uint16_t offset = (state->h << 8) | state->l;
    state->d = state->memory[offset];
  }
  break;
  case 0x57:
    UnimplementedInstruction(state);
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
    uint16_t offset = (state->h << 8) | state->l;
    state->e = state->memory[offset];
  }
  break;
  case 0x5E:
  {
    uint16_t offset = (state->h << 8) | (state->l);
    state->e = state->memory[offset];
  }
  break;
  case 0x5F:
    UnimplementedInstruction(state);
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
    uint16_t offset = (state->h << 8) | state->l;
    state->h = state->memory[offset];
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
  case 0x7D:
    UnimplementedInstruction(state);
    break;
  case 0x7E: // MOV A,M
  {
    uint16_t offset = (state->h << 8 | state->l);
    state->a = state->memory[offset];
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
    state->flags.cy = state->flags.ac = 0;
    state->flags.z = (state->a == 0);
    state->flags.s = (0x80 == (state->a & 0x80));
    state->flags.p = parity(state->a, 8);
  }
  break;

  case 0xB0:
    UnimplementedInstruction(state);
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
  case 0xB6:
    UnimplementedInstruction(state);
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
  case 0xC8:
    UnimplementedInstruction(state);
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
  case 0xCB:
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
  case 0xD8:
    UnimplementedInstruction(state);
    break;
  case 0xD9:
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
  case 0xDD:
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
    state->flags.cy = state->flags.ac = 0;
    state->flags.z = (state->a == 0);
    state->flags.s = (0x80 == (state->a & 0x80));
    state->flags.p = parity(state->a, 8);
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
  case 0xEB: // XCHG - 	H <-> D; L <-> E
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
  case 0xF5: // PUSH PSW - 	(sp-2)<-flags; (sp-1)<-A; sp <- sp - 2
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
  // Debugging information
  printf("\tC=%d,P=%d,S=%d,Z=%d,AC=%d\n", state->flags.cy, state->flags.p,
         state->flags.s, state->flags.z, state->flags.ac);
  // printf("\tA $%02x B $%02x C $%02x D $%02x E $%02x H $%02x L $%02x SP %04x\n",
  //        state->a, state->b, state->c, state->d,
  //        state->e, state->h, state->l, state->sp);
  printf("\tA $%02x BC $%02x%02x DE $%02x%02x HL $%02x%02x SP %04x PC %04x\n",
         state->a, state->b, state->c, state->d,
         state->e, state->h, state->l, state->sp, state->pc);
  return 0;
}

void ReadFileIntoMemoryAt(State8080 *state, char *filename, uint32_t offset)
// open the filename, insert the contents into memory at offset
// store this in the state memory
{
  FILE *f = fopen(filename, "rb"); // r = read, b = binary. turns the pointer into a value.
  if (f == NULL)
  {
    printf("error: Could not open %s\n", filename);
    exit(1); // if the read fails, throw an error
  }
  fseek(f, 0L, SEEK_END);
  // fseek changes the current file position associated with the stream to a new location in the file.
  // here we set an offset of 0, and say to start from the end of the file. not quite sure why tho...
  int fsize = ftell(f);
  // ftell returns the current file position indicator.
  fseek(f, 0L, SEEK_SET);
  // the file position indicator is where in the file the stream is currently reading or writing.
  // a stream is a generic interface for reading or writing (file, USB...)
  // get the size of the file and then set indicator back to start.
  uint8_t *buffer = &state->memory[offset];
  // put a pointer to the buffer into memory at this particular offset
  fread(buffer, fsize, 1, f);
  // read the file into that buffer
  fclose(f);
  // close the file :)
}

State8080 *Init8080(void)
{
  State8080 *state = calloc(1, sizeof(State8080));
  // calloc returns a pointer to enough free space for an array of arg_1 objects of the specified size.
  // this storage is initalised to zero.
  state->memory = malloc(0x10000); // 16K
  // malloc returns a pointer to n bytes of uninit storage
  return state;
}

int main(int argc, char **argv)
{
  int done = 0;
  State8080 *state = Init8080();

  // Space Invaders
  ReadFileIntoMemoryAt(state, "invaders.h", 0);
  ReadFileIntoMemoryAt(state, "invaders.g", 0x800);
  ReadFileIntoMemoryAt(state, "invaders.f", 0x1000);
  ReadFileIntoMemoryAt(state, "invaders.e", 0x1800);
  // memory map here http://www.emutalk.net/threads/38177-Space-Invaders

  // CPU Diagnostics (for testing purposes)
  // ReadFileIntoMemoryAt(state, "cpudiag.bin", 0x100);
  // // Fix first instruction to be JMP 0x100
  // state->memory[0] = 0xC3;
  // state->memory[1] = 0;
  // state->memory[2] = 0x01;
  // // Fix stack pointer from 0x6ad to 0x7ad (which is buggy apparently)
  // state->memory[368] = 0x7;
  // // Skip DAA test
  // state->memory[0x59C] = 0xC3;
  // state->memory[0x59D] = 0xC2;
  // state->memory[0x59E] = 0x05;

  while (1)
  {
    uint8_t *opcode = &state->memory[state->pc];
    // listen for key events here
    // set byte in there and then check it in IN

    if (*opcode == 0xDB) // IN
    {
      uint8_t port = opcode[1];
      state->a = MachineIN(state, port);
      state->pc++;
    }
    else if (*opcode == 0xD3) // OUT
    {
      uint8_t port = opcode[1];
      MachineOUT(state, port);
      state->pc++;
    }
    else
    {
      done = Emulate8080(state);
    }
  }
  return 0;
}