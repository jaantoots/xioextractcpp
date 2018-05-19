// -*- mode: c++ -*-
#ifndef OSC_MESSAGE_H
#define OSC_MESSAGE_H

#include <string>

template<class InputIt>
bool is_message (const InputIt &first, const InputIt &last);

class Message {
 public:
  template<class InputIt>
    Message (InputIt &first, const InputIt &last);
 private:
  std::string address;
  std::string type_tag;
};

#endif
