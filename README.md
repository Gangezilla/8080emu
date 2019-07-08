# 8080emu

## Discoveries

### Why do we bitshift and then do a logical or?

There's a lot of code that looks like this, ie bit shift a higher order byte, and then logical or it with the lower order byte.
`state->pc = (opcode[2] << 8) | opcode[1];`
We do this to construct a 16 bit offset by combining the Highest 8 bits with the Lowest 8 bits.
We do this because representing a 16 bit value requires two bytes. Left shifting puts H in the second (high) byte, and | combines it with the low byte, producing the offset.
It looks like this:

```
H =                           H7H6H5H4H3H2H1H0
H << 8 =     H7H6H5H4H3H2H1H0 0 0 0 0 0 0 0 0
H << 8 | L = H7H6H5H4H3H2H1H0 L7L6L5L4L3L2L1L0
```

### What are all the flags again?

The processor maintains internal flag bits which indicate the result of logical and arithmetic instructions. These are:

- Sign (S), set if negative result.
- Zero (Z), set if the result is zero.
- Parity (P), set if the number of 1 bits in the result is even.
- Carry (C), set if the last addition operation resulted in a carry, or if the last subtraction required a borrow.
- Auxilary Carry (AC or H), used for binary-coded decimal arithmetic.

### When we do a decrement, like 0x05, why do we set the sign flag like that?

The sign flag is set like so:

```
  uint8_t res = state->b - 1;
  state->flags.s = (0x80 == (res & 0x80));
```

This means we first do the calculation, then we do some compares. This one checks to see if 0x80 is the same as the result logical and'ed with 0x80. For example, if our res is 255.

```
res  (255): 1 0 0 0 0 0 0 0 0
0x80 (128): 0 1 1 1 1 1 1 1 1
res & 0x80: 0 0 0 0 0 0 0 0 0
```

We do this check because anding it will... TODO

### Why do we care about the parity flag?

As per: https://stackoverflow.com/questions/25707130/what-is-the-purpose-of-the-parity-flag-on-a-cpu

The parity flag is set if the number of 1 bits in the result is even. It's basically a leftover from the olden days to do parity checking in software which is a very simple error-detection scheme.

It was originally used by telegraphs, and other serial communication protocols. The idea was to count the number of set bits in a character and include an extra bit to indicate whether that character contained an even or odd number of set bits. That way, the receiver can count the number of bits, and verify that the extra "parity" bit indicated a successful transmission.

It ended up in CPU's to support serial communication hardware, basically.
