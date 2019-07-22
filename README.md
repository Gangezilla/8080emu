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

```c
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

```c
  uint8_t res = state->b - 1;
  state->flags.s = (0x80 == (res & 0x80));
```

This means we first do the calculation, then we do some compares. This one checks to see if 0x80 is the same as the result logical and'ed with 0x80. For example, if our res is 255.

```c
res  (255): 1 0 0 0 0 0 0 0 0
0x80 (128): 0 1 1 1 1 1 1 1 1
res & 0x80: 0 0 0 0 0 0 0 0 0
```

We do this check because 0x80 is 128, which is the max size of a signed integer. If it's bigger than this, we know we'd need to use a sign to represent this number.

### Why do we care about the parity flag?

As per: <https://stackoverflow.com/questions/25707130/what-is-the-purpose-of-the-parity-flag-on-a-cpu>

The parity flag is set if the number of 1 bits in the result is even. It's basically a leftover from the olden days to do parity checking in software which is a very simple error-detection scheme.

It was originally used by telegraphs, and other serial communication protocols. The idea was to count the number of set bits in a character and include an extra bit to indicate whether that character contained an even or odd number of set bits. That way, the receiver can count the number of bits, and verify that the extra "parity" bit indicated a successful transmission.

It ended up in CPU's to support serial communication hardware, basically.

### What's the deal with the DAD instructions?

So we get the 16 bit reg values. then we add them together. Then we put into the H register a value that's calculated like this:

```c
  res (7728):          1111000110000
  res & 0xff00:        110000
  (res & 0xff00) >> 8: 11110
```

We shift it back to the right to basically undo how we got H and D in the first place. We do our calculations with 0xff00 because it's the max number we can store in 2 bytes, and we use 0xff with register L because it's the max value in 1 byte. We're basically undoing the transforms we did earlier. We set the carry flag with 0xffff0000 because that's the max number that can be stored by 4 bytes.

### What's a PSW? What's going on in the POP PSW (0xf1) instruction?

PSW is a special register pair of the accumulator and the flags. PSW stands for program status word, and it's apparently the piece of memory that contains the status flags. It's a piece of memory that is designed to keep track of the current state of the system. It's formed by treating the accumulator and Flags as one single 16 bit register.

POP PSW pops both accumulator and the flags off the stack. The content of the memory location specified in the stack pointer is used to restore the condition flags. The content of the stack pointer plus one is moved to register A, and the stack pointer itself is incremented by 2.

I think we want to POP PSW to restore the flags back to a previous state. The implementation is as it is, because when we construct the 16bit PSW, we can get the individual flags by checking if a binary position of that number is equivalent to a particular number, such as 0x01 or 0x02.

### What's register M refer to?

Register M is the 16 bit register formed by combining registers H and L. The M stands for Memory. When we see it get used, such as in 0x7E (MOV A,M) we want to move what's in register M (HL) into A. We get the offset of combining H and L, then access the memory at that location and assign it to A.

### Why do we rotate?

You've got some instructions like RRC which is where you rotate the accumulator to the right, meaning `0b00010000` becomes `0b00001000`. This is actually super cool! Doing this lets you multiply (left shift) and divide (right shift) by two.

Our implementation (0x0F) works like so:

`uint8_t x = state->a;`
Let's say x is 27, or 0b0001_1010
`state->a = ((x & 1) << 7) | (x >> 1);`
`(x & 1)` is `(0b0001_1011 & 0b0000_0001)`
`0b0000_00001 << 7` is 128, or `0b1000_0000`
`0b0001_1011 >> 1 = 0b0000_1101`
`0b0001_1011 | 0b0000_1101 = 0b0001_1111`
Tjhen we set the carry flag like this,
`state->flags.cy = (1 == (x & 1));`
Which looks like this:
`1 == (0b0000_00001 & 1) = 1`

You can also use this to multiply or divide by any arbitrary number. For example, you multiply by decomposing the numbers by powers of two. Eg:

```c
21 * 5 = 0b10101 * 0b101
       = 0b10101 * ((1 * 2^2) + (0 + 2^1) + (1 * 2^0))
       = 0b10101 * (2^2 + 0b10101 * 2^0)
       = 0b10101 << 2 + 0b10101 << 0
       = (0b10101 * 4) + (0b10101 * 1)
       = 0b10101 * 5
       = 21 * 5
```

(resourced from <https://stackoverflow.com/questions/2776211/how-can-i-multiply-and-divide-using-only-bit-shifting-and-adding)>

### What's going on with PUSH PSW (0xF5)?

We're constructing a new 8 bit number by moving each of the flags across so that when we add them up, it looks something like this:

```c
z  = 0b0001 << 0 = 0b0001
s  = 0b0000 << 1 = 0b0000
p  = 0b0001 << 2 = 0b0100
cy = 0b0001 << 3 = 0b1000
ac = 0b0000 << 4 = 0b0000
                 = 0b1101
```

So as you can see, it allows us to line the flags up so we can generate a new number with them. I'm not sure why they're in the order they are though. Maybe something to do with how they get stored or something.

### Why do we want to enable interrupts? (0xFB)

Interrupts are included on many CPUs as a way of improving the processor's efficiency. The Intel 8080 handbook gives an example of interfacing with a printer, where you output a byte of data, but the printer may take a longer time to process that byte. You could then use interrupts to space out the way that you send these bytes. It's implemented externally, so the printer in this example can request an interrupt. This means that when the printer is processing, the CPU can continue doing it's own thing. Then when the printer is ready, it can interrupt the CPU and request more data.

It's used in the Space Invaders to display pixels on screen. According to this page <http://computerarcheology.com/Arcade/SpaceInvaders/Hardware.html>, Space Invaders has 2 video interrupts. One is at the end of the frame, and one is generated in the middle of the screen. The game draws the top half of the screen after it receives the mid-screen interrupt, and then draws the bottom half after it receives the end interrupt, which is done to avoid screen tearing.

### What's a "word"?

A word is a unit of data used in processor design. It's fixed size, and handled as part of the instruction set. In this instance, we have 8 bit words, and I think sometimes it's used in the context of 16 bit words too. <https://en.wikichip.org/wiki/intel/mcs-80/8080>

### What's the shift register component of the Space Invaders machine?

The 8080 instruction set does not include opcodes for bit shifting multiple bits. However, you need to do this because you're shifting an 8 bit pixel image into a 16 bit word to get it into the right position on screen. To get around this, Space Invaders has a dedicated shift register component. So instead of needing tens of 8080 instructions to implement a multi-bit/multi-byte shift, the shift register can do it in a few instructions. It's broken down pretty well here <http://computerarcheology.com/Arcade/SpaceInvaders/Hardware.html>, which I'll replicate here:

```
f              0	bit
xxxxxxxxyyyyyyyy

Writing to port 4 shifts x into y, and the new value into x, eg.
$0000,
write $aa -> $aa00,
write $ff -> $ffaa,
write $12 -> $12ff, ..

Writing to port 2 (bits 0,1,2) sets the offset for the 8 bit result, eg.
offset 0:
rrrrrrrr		result=xxxxxxxx
xxxxxxxxyyyyyyyy

offset 2:
  rrrrrrrr	result=xxxxxxyy
xxxxxxxxyyyyyyyy

offset 7:
       rrrrrrrr	result=xyyyyyyy
xxxxxxxxyyyyyyyy

Reading from port 3 returns said result.
```

### Tell me about bit manipulation.

Grabbed from here <http://www.goldsborough.me/bits/c++/low-level/problems/2015/10/11/23-52-02-bit_manipulation/>

#### Setting bits

You will want to set a bit when you want to change one of the individual bits in a byte to 1. The easiest way to do this is the OR operation, because it will always set a bit to 1. To set the nth bit (starting at 0) of a value x: `x = x | (1 << n)`

```
0000 | (1 << 0) = 0000 | 0001 = 0001
1000 | (1 << 1) = 1000 | 0001 = 1010
```

We also see this in some places, like this (let's assume x is 1):

```
x |= 0x20 // set bit 5 of x
x = x | 0x20
0b0000_0001 | 0b0010_0000 = 0b0010_0001
```

This retains the original bit, and also sets the new one.

#### Clearing bits

Clearing a bit is the opposite of setting one, that is, when you want to change an individual bit to 0. The easiest way to do this is with the AND operation. To clear a bit we use the AND and NOT operators. You create a bit-mask (which defines which bits you want to keep and which you want to clear) where all bits are set EXCEPT for the one you want to clear. The formula of this looks like: `x &= ~(1 << n)`.

```
0111 & ~(1 << 0) = 0111 & 1110 = 0110
0100 & ~(1 << 2) = 0100 & 1101 = 0000 // is 64, but its a 4 bit number so we lose precision...
```

You'll also see examples like this sometimes too `6 &= 0xDF`. This is the same as above for clearing bit 5, it's just the binary inverse already, so you don't need to and it.

```
  6 = 0b0000_0110
  0xDF = 0b1101_1111, but ~(0xDF) = 0010_0000 which is the 5th bit
```

#### Toggling bits

We use XOR with 1 to toggle a bit, which will always flip a 0 to a 1, and a 1 to a 0. The formula for this looks like `x ^= (1 << n)`. Side note, XOR is a logical operation short for exclusive or if ONLY one bit is set. It's function table looks like this:

```
a | b | a ^ b |
--|---|-------|
0 | 0 |   0   |
0 | 1 |   1   |
1 | 0 |   1   |
1 | 1 |   0   |
---------------
```

Examples of this look like:

```
0010 ^ (1 << 1) = 0010 ^ 0010 = 0000
0110 ^ (1 << 0) = 0110 ^ 0010 = 0111
```

## Helpful Resources

- <http://www.nj7p.info/Manuals/PDFs/Intel/9800153B.pdf> - Intel 8080 manual. Really interesting and has a lot of really valuable references. Some of the instruction descriptions are a bit hard to understand though.
- <http://www.emulator101.com> - tutorial for a lot of this, as well as inspiration for some of the instruction implementation.
- <http://www.classiccmp.org/dunfield/r/8080.txt> - some explanation of the instructions, when the manual is a bit too terse.
- <https://ia601202.us.archive.org/25/items/IntroductionTo80808085AssemblyLanguageProgramming/introduction%20to%208080%208085%20assembly%20language%20programming.pdf>, great textbook which goes a bit more in depth on assembly language for the 8080. I found it was helpful looking at it from the other side sometimes.
- <http://pastraiser.com/cpu/i8080/i8080_opcodes.html>
- <http://computerarcheology.com/Arcade/SpaceInvaders/Hardware.html>
- <https://www.walkofmind.com/programming/side/side.htm>

Plus a few other emulators that I've had a look at during this process:

- <https://github.com/superzazu/8080/blob/master/i8080.c>
- <https://github.com/pbodsk/8080emulator/blob/master/emulator/emulator.c>
- <https://github.com/forbesb/eightyeighty/blob/master/8080CPU.c>
