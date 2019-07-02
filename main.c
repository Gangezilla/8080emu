#include <stdio.h>
#include <stdlib.h>
// #include <string.h>
// #include <stdint.h>

// uint8 is an unsigned char, can hold values  0 - 255.
typedef struct Flags
{
  uint8_t z : 1;
  uint8_t s : 1;
  uint8_t p : 1;
  uint8_t cy : 1;
  uint8_t ac : 1;
  uint8_t pad : 3;
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
  uint16_t sp;
  uint16_t pc;
  uint8_t *memory;
  struct Flags flags;
  uint8_t int_enable;
} State8080;

void UnimplementedInstruction(State8080 *state, char *opcode)
{
  printf("Error: Unimplemented instruction, %c\n", *opcode);
  exit(1);
}

int Emulate8080(State8080 *state)
{
  // * turns a pointer into a value
  // & turns a value into a pointer
  // -> is used to access members of a struct when theyre a pointer.
  unsigned char *opcode = &state->memory[state->pc];

  switch (*opcode)
  {
  case 0x00:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x01:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x02:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x03:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x04:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x05:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x06:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x07:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x08:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x09:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x0A:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x0B:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x0C:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x0D:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x0E:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x0F:
    UnimplementedInstruction(state, *opcode);
    break;

  case 0x10:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x11:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x12:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x13:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x14:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x15:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x16:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x17:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x18:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x19:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x1A:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x1B:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x1C:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x1D:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x1E:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x1F:
    UnimplementedInstruction(state, *opcode);
    break;

  case 0x20:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x21:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x22:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x23:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x24:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x25:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x26:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x27:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x28:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x29:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x2A:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x2B:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x2C:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x2D:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x2E:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x2F:
    UnimplementedInstruction(state, *opcode);
    break;

  case 0x30:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x31:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x32:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x33:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x34:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x35:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x36:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x37:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x38:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x39:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x3A:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x3B:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x3C:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x3D:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x3E:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x3F:
    UnimplementedInstruction(state, *opcode);
    break;

  case 0x40:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x41:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x42:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x43:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x44:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x45:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x46:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x47:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x48:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x49:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x4A:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x4B:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x4C:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x4D:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x4E:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x4F:
    UnimplementedInstruction(state, *opcode);
    break;

  case 0x50:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x51:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x52:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x53:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x54:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x55:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x56:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x57:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x58:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x59:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x5A:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x5B:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x5C:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x5D:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x5E:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x5F:
    UnimplementedInstruction(state, *opcode);
    break;

  case 0x60:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x61:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x62:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x63:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x64:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x65:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x66:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x67:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x68:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x69:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x6A:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x6B:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x6C:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x6D:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x6E:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x6F:
    UnimplementedInstruction(state, *opcode);
    break;

  case 0x70:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x71:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x72:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x73:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x74:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x75:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x76:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x77:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x78:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x79:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x7A:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x7B:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x7C:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x7D:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x7E:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x7F:
    UnimplementedInstruction(state, *opcode);
    break;

  case 0x80:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x81:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x82:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x83:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x84:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x85:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x86:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x87:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x88:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x89:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x8A:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x8B:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x8C:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x8D:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x8E:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x8F:
    UnimplementedInstruction(state, *opcode);
    break;

  case 0x90:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x91:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x92:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x93:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x94:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x95:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x96:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x97:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x98:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x99:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x9A:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x9B:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x9C:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x9D:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x9E:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0x9F:
    UnimplementedInstruction(state, *opcode);
    break;

  case 0xA0:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0xA1:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0xA2:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0xA3:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0xA4:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0xA5:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0xA6:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0xA7:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0xA8:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0xA9:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0xAA:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0xAB:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0xAC:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0xAD:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0xAE:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0xAF:
    UnimplementedInstruction(state, *opcode);
    break;

  case 0xB0:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0xB1:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0xB2:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0xB3:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0xB4:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0xB5:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0xB6:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0xB7:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0xB8:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0xB9:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0xBA:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0xBB:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0xBC:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0xBD:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0xBE:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0xBF:
    UnimplementedInstruction(state, *opcode);
    break;

  case 0xC0:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0xC1:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0xC2:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0xC3:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0xC4:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0xC5:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0xC6:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0xC7:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0xC8:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0xC9:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0xCA:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0xCB:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0xCC:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0xCD:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0xCE:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0xCF:
    UnimplementedInstruction(state, *opcode);
    break;

  case 0xD0:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0xD1:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0xD2:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0xD3:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0xD4:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0xD5:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0xD6:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0xD7:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0xD8:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0xD9:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0xDA:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0xDB:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0xDC:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0xDD:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0xDE:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0xDF:
    UnimplementedInstruction(state, *opcode);
    break;

  case 0xE0:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0xE1:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0xE2:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0xE3:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0xE4:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0xE5:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0xE6:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0xE7:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0xE8:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0xE9:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0xEA:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0xEB:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0xEC:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0xED:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0xEE:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0xEF:
    UnimplementedInstruction(state, *opcode);
    break;

  case 0xF0:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0xF1:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0xF2:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0xF3:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0xF4:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0xF5:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0xF6:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0xF7:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0xF8:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0xF9:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0xFA:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0xFB:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0xFC:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0xFD:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0xFE:
    UnimplementedInstruction(state, *opcode);
    break;
  case 0xFF:
    UnimplementedInstruction(state, *opcode);
    break;
  }
}

void ReadFileIntoMemoryAt(State8080 *state, char *filename, uint32_t offset)
{
}

State8080 *Init8080(void)
{
}

int main(int argc, char **argv)
{
  int done = 0;
  State8080 *state = Init8080();

  ReadFileIntoMemoryAt(state, "invaders.h", 0);
  ReadFileIntoMemoryAt(state, "invaders.g", 0x800);
  ReadFileIntoMemoryAt(state, "invaders.f", 0x1000);
  ReadFileIntoMemoryAt(state, "invaders.e", 0x1800);
  // memory map here http://www.emutalk.net/threads/38177-Space-Invaders

  while (done == 0)
  {
    done = Emulate8080(state);
  }
  return 0;
}