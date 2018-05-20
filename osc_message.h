// -*- mode: c++ -*-
#ifndef OSC_MESSAGE_H
#define OSC_MESSAGE_H

#include <string>
#include <vector>

template<class InputIt>
bool is_message (const InputIt &first, const InputIt &last);

struct Argument {
  int32_t i;
  float f;
  std::string s;
  std::string b;
  long double t;
};

class Message {
 public:
  template<class InputIt>
    Message (InputIt &first, const InputIt &last);
 private:
  std::string address;
  std::string types;
  std::vector<Argument> args;
};

#endif
