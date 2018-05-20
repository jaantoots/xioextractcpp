#include <cassert>
#include <cstdio>
#include <string>

#include "osc_bundle.h"
#include "osc_message.h"
#include "osc_types.h"

/* Exception for errors encountered during message parsing */
struct BundleException : public std::exception {
  std::string msg;
  BundleException(std::string s) : msg(s) {}
  const char * what () const throw () { return msg.c_str(); }
};

/* Construct Bundle object from iterators */
template<class InputIt>
Bundle::Bundle (InputIt &first, const InputIt &last) {
  // OSC String "#bundle" begins bundle
  std::string b = get_string(first, last);
  assert(b == "#bundle");
  // OSC Time Tag is next
  time = get_time(first, last);
  // Parse OSC Bundle Elements
  while (first < last) {
    continue;
  }
  assert(first == last);
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
