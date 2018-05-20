#include <cstdio>
#include <cstdlib>
#include <vector>

#include "slip.h"

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