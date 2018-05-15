#include <cassert>
#include <string>
#include <algorithm>

#define OSC_STRING_MOD 4

template<class Iter>
struct osc_string { std::string str; Iter end; };

template<class Iter>
osc_string<Iter> get_string (Iter first, Iter last) {
  Iter end = std::find(first, last, '\0');
  std::string str(first, end);
  assert(end > first);
  end += ((first - end) % OSC_STRING_MOD) + OSC_STRING_MOD;
  assert(end <= last);
  return osc_string<Iter> { str, end };
}

int main (int argc, char *argv[]) {
  return 0;
}
