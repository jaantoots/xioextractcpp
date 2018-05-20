// -*- mode: c++ -*-
#ifndef OSC_BUNDLE_H
#define OSC_BUNDLE_H

#include <vector>

#include "osc_message.h"

struct Element {
  Element(char t) : type(t) {}
  char type;
  class Bundle *b;
  Message *m;
};

class Bundle {
 public:
  template<class InputIt>
  Bundle (InputIt &first, const InputIt &last);
  void put_csv (FILE* fp);
 private:
  long double time;
  std::vector<Element> args;
};

#endif
