#include <cassert>
#include <string>

#include "osc_types.h"

#include "osc_message.h"

struct MessageException : public std::exception {
  std::string msg;
  MessageException(std::string s) : msg(s) {}
  const char * what () const throw () { return msg.c_str(); }
};

template<class Iter>
bool is_message (const Iter &first, const Iter &last) {
  assert(first < last);
  return (*first == '/');
}

template<class Iter>
Message::Message (Iter &first, const Iter &last) {
  assert(is_message(first, last));
  address = get_string(first, last);
  assert(*first == ',');
  type_tag = get_string(first, last);
  for (const char c : type_tag.substr(1)) {
    switch(c) {
    case 'i' : get_int32(first, last);
      break;
    case 'f' : get_float(first, last);
      break;
    case 's' : get_string(first, last);
      break;
    case 'b' : get_blob(first, last);
      break;
    case 't' : get_time(first, last);
      break;
    default : throw MessageException("Unrecognised type tag " + c);
    }
  }
}

int main (int argc, char *argv[]) {
  std::string in("#bundle\0\0\0\0\0\0\0\0\1", 16);
  std::string::iterator it = in.begin();
  const std::string c1 = get_string(it, in.end());
  printf("%s\n%d\n", c1.c_str(), (int) (it - in.begin()));
  const long double c2 = get_time(it, in.end());
  printf("%.11Lf\n%d\n", c2, (int) (it - in.begin()));

  std::string is("/addr\0\0\0,s\0\0bundle\0\0", 20);
  std::string::iterator itt = is.begin();
  Message(itt, is.end());
  return 0;
}
