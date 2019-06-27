#include <stdio.h>
// #include <string.h>
// #include <stdlib.h>
// #include <stdint.h>

void ReadFileIntoMemoryAt(State8080 *state, char *filename, uint32_t offset)
{
  FILE *f = fopen(filename, "rb"); 
  // 2nd arg is mode, rb is 'read' 'binary'
  if (f == NULL)
  {
    printf("error: Couldn't open %s\n", filename);
    exit(1);
  }
  fseek(f, 0l, SEEK_END); 
  // fseek sets the position indicator with this stream to a new position.
  // 0l is a long with all bits set to 0.
  // SEEK_END is just the end of the file.
  int fsize = ftell(f);
  // ftell gets the current position in the stream.
  fseek(f, 0l, SEEK_SET);
  // SEEK_SET is the beginning of the file.
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