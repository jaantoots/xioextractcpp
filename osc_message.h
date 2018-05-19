#ifndef OSC_MESSAGE_H
#define OSC_MESSAGE_H

#include <string>

template<class Iter>
bool is_message (const Iter &first, const Iter &last);

class Message {
 public:
  template<class Iter>
    Message (Iter &first, const Iter &last);
 private:
  std::string address;
  std::string type_tag;
};

#endif
