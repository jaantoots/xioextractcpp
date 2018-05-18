#include <cstdint>
#include <cassert>
#include <cmath>
#include <string>
#include <algorithm>

#define OSC_MOD 4
#define NTP_DELTA 2208988800

/* Get OSC_MOD chars as type T from Iter */
template<class T, class Iter>
T get_from_c (Iter &first, const Iter &last) {
  assert(last - first >= OSC_MOD);
  union mix32_t {
    T t;
    uint32_t u;
    char c[OSC_MOD];
  } m;
  copy_n(first, OSC_MOD, m.c);
  first += OSC_MOD;
  // Convert from big endian to system
  m.u = be32toh(m.u);
  return m.t;
}

/* Get signed 32-bit integer from chars */
template<class Iter>
int32_t get_int32 (Iter &first, const Iter &last) {
  return get_from_c<int32_t>(first, last);
}

/* Get unsigned 32-bit integer from chars */
template<class Iter>
uint32_t get_uint32 (Iter &first, const Iter &last) {
  return get_from_c<uint32_t>(first, last);
}

/* Get timestamp from unsigned 64-bit NTP timestamp */
template<class Iter>
long double get_time (Iter &first, const Iter &last) {
  uint32_t secs = get_uint32(first, last);
  uint32_t frac = get_uint32(first, last);
  // `long double` *should* be 80-bit float
  long double t = secs + (long double) frac / (1ul << 32);
  t -= NTP_DELTA; // NTP time starts at 1 Jan 1900
  return t;
}

/* Get IEEE-754 32 bit float */
template<class Iter>
float get_float (Iter &first, const Iter &last) {
  assert(std::numeric_limits<float>::is_iec559);
  return get_from_c<float>(first, last);
}

/* Get a \0 terminated and padded string */
template<class Iter>
std::string get_string (Iter &first, const Iter &last) {
  Iter end = std::find(first, last, '\0');
  std::string str(first, end);
  end += ((first - end) % OSC_MOD) + OSC_MOD;
  assert(end <= last);
  first = end;
  return str;
}

/* Get arbitrary length binary blob */
template<class Iter>
std::string get_blob (Iter &first, const Iter &last) {
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
  long double c2 = get_time(it, in.end());
  printf("%.11Lf\n%d\n", c2, (int) (it - in.begin()));
}
