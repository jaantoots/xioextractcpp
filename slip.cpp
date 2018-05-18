#include <cstdio>
#include <cstdlib>
#include <vector>

#include "slip.h"

#define SLIP_END '\xc0'
#define SLIP_ESC '\xdb'
#define SLIP_ESC_END '\xdc'
#define SLIP_ESC_ESC '\xdd'


std::vector<unsigned char> get_dgram (FILE* fp) {
  std::vector<unsigned char> dgram;
  bool esc = false;
  char c;
  int ci; // note: int, not char, required to handle EOF
  while ((ci = fgetc(fp)) != EOF) {
    c = ci;
    if (c == SLIP_END)
      break;
    if (c == SLIP_ESC) {
      esc = true;
      continue;
    }
    if (esc)
      switch(c) {
      case SLIP_ESC_END : c = SLIP_END;
        break;
      case SLIP_ESC_ESC : c = SLIP_ESC;
        break;
      }
    dgram.push_back(c);
    esc = false;
  }
  return dgram;
}

int put_dgram (const std::vector<unsigned char> &dgram, FILE* fp) {
  for (char c : dgram) {
    switch(c) {
    case SLIP_END : fputc(SLIP_ESC, fp); c = SLIP_ESC_END;
      break;
    case SLIP_ESC : fputc(SLIP_ESC, fp); c = SLIP_ESC_ESC;
      break;
    }
    fputc(c, fp);
  }
  fputc(SLIP_END, fp);
  return 0;
}

int main (int argc, char *argv[]) {
  if (argc != 2) {
    printf("usage: %s FILE\n", argv[0]);
    return 1;
  }

  FILE* fp = fopen(argv[1], "r");
  if(!fp) {
    perror("File opening failed");
    return EXIT_FAILURE;
  }

  while (!(feof(fp))) {
    std::vector<unsigned char> dgram = get_dgram(fp);
    if (!(dgram.empty()))
      put_dgram(dgram, stdout);
  }

  if (ferror(fp))
    puts("I/O error when reading");
  fclose(fp);
  return 0;
}
