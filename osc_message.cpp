#include <cassert>
#include <cstdio>
#include <string>

#include "osc_types.h"

#include "osc_message.h"

struct MessageException : public std::exception {
  std::string msg;
  MessageException(std::string s) : msg(s) {}
  const char * what () const throw () { return msg.c_str(); }
};

template<class InputIt>
bool is_message (const InputIt &first, const InputIt &last) {
  assert(first < last);
  return (*first == '/');
}

template<class InputIt>
Message::Message (InputIt &first, const InputIt &last) {
  assert(is_message(first, last));
  address = get_string(first, last);
  assert(*first == ',');
  types = get_string(first, last).substr(1);
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
}

void Message::put_csv (FILE* fp) {
  fprintf(fp, "%s", address.c_str());
  for (const Argument arg : args) {
    switch(arg.type) {
    case 'i' : fprintf(fp, ",%d", arg.i);
      break;
    case 'f' : fprintf(fp, ",%.11g", arg.f);
      break;
    case 's' : fprintf(fp, ",%s", arg.s.c_str());
      break;
    case 'b' : fprintf(fp, ",0x");
      for (unsigned char c : arg.b)
        fprintf(fp, "%.2hhx", c);
      break;
    case 't' : fprintf(fp, ",%.11Lf", arg.t);
      break;
    default : assert(!"type tag with printing not implemented");
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

  std::string is("/addr\0\0\0,stb\0\0\0\0bundle\0\0\xff\xff\xff\xff\xff\xff\xff\xff\0\0\0\2\x0f\x00\0\0", 40);
  std::string::iterator itt = is.begin();
  Message msg = Message(itt, is.end());
  msg.put_csv(stdout);
  printf("\n");
  return 0;
}
