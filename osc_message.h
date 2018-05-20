// -*- mode: c++ -*-
#ifndef OSC_MESSAGE_H
#define OSC_MESSAGE_H

#include <cassert>
#include <cstdint>
#include <string>
#include <vector>

#include "osc_types.h"

/* OSC Message class that gets its content from iterators. Contains 0
   or more Arguments. */

template<class InputIt>
bool is_message (const InputIt &first, const InputIt &last);

struct Argument {
  Argument(char t) : type(t) {}
  char type;
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
  void put_csv (FILE* fp) const;
private:
  std::string address;
  std::string types;
  std::vector<Argument> args;
};

/* Exception for errors encountered during message parsing */
struct MessageException : public std::exception {
  std::string msg;
  MessageException(std::string s) : msg(s) {}
  const char * what () const throw () { return msg.c_str(); }
};

/* Return true if a message starts at `first` */
template<class InputIt>
bool is_message (const InputIt &first, const InputIt &last) {
  assert(first < last);
  return (*first == '/');
}

/* Construct Message object from iterators */
template<class InputIt>
Message::Message (InputIt &first, const InputIt &last) {
  // OSC Address Pattern begins message
  assert(is_message(first, last));
  address = get_string(first, last);
  // OSC Type Tag String specifies arguments
  assert(*first == ',');
  types = get_string(first, last).substr(1);
  // Parse OSC Arguments
  for (const char c : types) {
    Argument arg(c);
    switch(c) {
    case 'i' : arg.i = get_int32(first, last);
      break;
    case 'f' : arg.f = get_float(first, last);
      break;
    case 's' : arg.s = get_string(first, last);
      break;
    case 'b' : arg.b = get_blob(first, last);
      break;
    case 't' : arg.t = get_time(first, last);
      break;
    default : throw MessageException(std::string("Unrecognised type tag ") + c);
    }
    args.push_back(arg);
  }
  assert(first == last);
}

#endif
