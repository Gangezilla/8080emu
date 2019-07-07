# 8080emu

* Load ROM into memory
* Set up different structs, like we did in the rust emu.
* power up the different components of the processor.
* Start going through the rom code, big switch statement and then do things with it.

## Discoveries

There's a lot of code that looks like this, ie bit shift a higher order byte, and then logical or it with the lower order byte.
`state->pc = (opcode[2] << 8) | opcode[1];`
We do this to construct a 16 bit offset by combining the Highest 8 bits with the Lowest 8 bits.
We do this because representing a 16 bit value requires two bytes. Left shifting puts H in the second (high) byte, and | combines it with the low byte, producing the offset.
It looks like this:
  H =                           H7H6H5H4H3H2H1H0
  H << 8 =     H7H6H5H4H3H2H1H0  0 0 0 0 0 0 0 0
  H << 8 | L = H7H6H5H4H3H2H1H0 L7L6L5L4L3L2L1L0
