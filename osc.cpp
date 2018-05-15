#include <cstdint>
#include <cassert>
#include <string>
#include <algorithm>

#define OSC_INT_WIDTH 4
#define OSC_FLOAT_WIDTH 4
#define OSC_STRING_MOD 4
#define OSC_BLOB_MOD 4
#define NTP_DELTA 2208988800

template<class Iter>
int32_t get_int32 (Iter &first, const Iter &last) {
  assert(last - first >= OSC_INT_WIDTH);
  int32_t int32 = *first << 24 | *(++first) << 16 | *(++first) << 8 | *(++first);
  ++first;
  return int32;
}

template<class Iter>
uint32_t get_uint32 (Iter &first, const Iter &last) {
  assert(last - first >= OSC_INT_WIDTH);
  uint32_t int32 = *first << 24 | *(++first) << 16 | *(++first) << 8 | *(++first);
  ++first;
  return int32;
}

template<class Iter>
double get_time (Iter &first, const Iter &last) {
  uint32_t secs = get_uint(first, last);
  uint32_t fsecs = get_uint(first, last);
  secs -= NTP_DELTA; // NTP time starts at 1 Jan 1900
  return (double) secs + (double) fsecs / (1ul << 32);
}

template<class Iter>
float get_float (Iter &first, const Iter &last) {
  assert(std::numeric_limits<float>::is_iec559);
  assert(last - first >= OSC_FLOAT_WIDTH);
  union mix32_t {
    float f;
    char c[OSC_FLOAT_WIDTH];
  } m;
  for (int i = 0; i < OSC_FLOAT_WIDTH; ++i)
    m.c[i] = *(first++);
  return m.f;
}

template<class Iter>
std::string get_string (Iter &first, const Iter &last) {
  Iter end = std::find(first, last, '\0');
  std::string str(first, end);
  end += ((first - end) % OSC_STRING_MOD) + OSC_STRING_MOD;
  assert(end <= last);
  first = end;
  return str;
}

template<class Iter>
std::string get_blob (Iter &first, const Iter &last) {
  long count = get_int32(first, last);
  assert(last - first >= count);
  std::string str(first, first + count);
  first += ((-count % OSC_BLOB_MOD) + OSC_BLOB_MOD) % OSC_BLOB_MOD;
  assert(first <= last);
  return str;
}

int main (int argc, char *argv[]) {
  std::string in("#bundle\0\0\0\0\1", 12);
  std::string::iterator it = in.begin();
  std::string c1 = get_string(it, in.end());
  printf("%s\n%d\n", c1.c_str(), (int) (it - in.begin()));
  float c2 = get_float(it, in.end());
  printf("%f\n%d\n", c2, (int) (in.size()));
}
