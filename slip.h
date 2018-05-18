#ifndef SLIP_H
#define SLIP_H

#include <cstdio>
#include <vector>

std::vector<unsigned char> get_dgram (FILE* fp);

int put_dgram (const std::vector<unsigned char> &dgram, FILE* fp);

#endif
