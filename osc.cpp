#include <cstdint>
#include <cassert>
#include <cmath>
#include <string>
#include <algorithm>

#define OSC_MOD 4
#define NTP_DELTA 2208988800ll

template<class T, class Iter>
T get_from_c (Iter &first, const Iter &last) {
  // Get OSC_MOD chars as type T
  assert(last - first >= OSC_MOD);
  union mix32_t {
    T t;
    uint32_t u;
    char c[OSC_MOD];
  } m;
  copy_n(first, OSC_MOD, m.c);
  first += OSC_MOD;
  m.u = be32toh(m.u);
  return m.t;
}

template<class Iter>
int32_t get_int32 (Iter &first, const Iter &last) {
  // Get signed 32-bit integer from chars
  return get_from_c<int32_t>(first, last);
}

template<class Iter>
uint32_t get_uint32 (Iter &first, const Iter &last) {
  // Get unsigned 32-bit integer from chars
  return get_from_c<uint32_t>(first, last);
}

struct timetag {
  long long secs;
  long long frac;
};

template<class Iter>
timetag get_time (Iter &first, const Iter &last) {
  // Get timestamp from unsigned 64-bit NTP timestamp
  uint32_t secs = get_uint32(first, last);
  uint32_t frac = get_uint32(first, last);
  timetag t = { secs, frac };
  t.secs -= NTP_DELTA; // NTP time starts at 1 Jan 1900
  return t;
}

template<class Iter>
float get_float (Iter &first, const Iter &last) {
  // Get IEEE-754 32 bit float
  assert(std::numeric_limits<float>::is_iec559);
  return get_from_c<float>(first, last);
}

template<class Iter>
std::string get_string (Iter &first, const Iter &last) {
  // Get a \0 terminated and padded string
  Iter end = std::find(first, last, '\0');
  std::string str(first, end);
  end += ((first - end) % OSC_MOD) + OSC_MOD;
  assert(end <= last);
  first = end;
  return str;
}

template<class Iter>
std::string get_blob (Iter &first, const Iter &last) {
  // Get arbitrary length binary blob
  long count = get_int32(first, last);
  assert(last - first >= count);
  std::string str(first, first + count);
  first += ((-count % OSC_MOD) + OSC_MOD) % OSC_MOD;
  assert(first <= last);
  return str;
}

int main (int argc, char *argv[]) {
  std::string in("#bundle\0\0\0\0\0\0\0\0\1", 16);
  std::string::iterator it = in.begin();
  std::string c1 = get_string(it, in.end());
  printf("%s\n%d\n", c1.c_str(), (int) (it - in.begin()));
  timetag c2 = get_time(it, in.end());
  printf("%.11Lf\n%d\n", (long double) c2.secs + (long double) c2.frac / (1ul << 32), (int) (it - in.begin()));
}
