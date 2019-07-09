# 8080emu

## Overview of 8080

TODO

## FAQ (by me)

### Why do we bitshift and then do a logical or so often?

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

### What's the deal with the DAD instructions?

So we get the 16 bit reg values. then we add them together. Then we put into the H register a value that's calculated like this:

```
  res (7728):          1111000110000
  res & 0xff00:        110000
  (res & 0xff00) >> 8: 11110
```

We shift it back to the right to basically undo how we got H and D in the first place. We do our calculations with 0xff00 because it's the max number we can store in 2 bytes, and we use 0xff with register L because it's the max value in 1 byte. We're basically undoing the transforms we did earlier. We set the carry flag with 0xffff0000 because that's the max number that can be stored by 4 bytes.

### What's a PSW? What's going on in the POP PSW (0xf1) instruction?

PSW is a special register pair of the accumulator and the flags. PSW stands for program status word, and it's apparently the piece of memory that contains the status flags. It's a piece of memory that is designed to keep track of the current state of the system. It's formed by treating the accumulator and Flags as one single 16 bit register.

POP PSW pops both accumulator and the flags off the stack. The content of the memory location specified in the stack pointer is used to restore the condition flags. The content of the stack pointer plus one is moved to register A, and the stack pointer itself is incremented by 2.

I think we want to POP PSW to restore the flags back to a previous state. The implementation is as it is, because when we construct the 16bit PSW, we can get the individual flags by checking if a binary position of that number is equivalent to a particular number, such as 0x01 or 0x02.
