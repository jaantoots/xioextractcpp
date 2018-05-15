#ifndef SLIP_H
#define SLIP_H

#include <cstdio>
#include <vector>

std::vector<char> get_dgram (FILE* fp);

int put_dgram (const std::vector<char> &dgram, FILE* fp);

#endif
