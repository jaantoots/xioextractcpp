// -*- mode: c++ -*-
#ifndef OSC_TYPES_H
#define OSC_TYPES_H

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <string>

/* Functions for getting specified types of data using the start and
   end iterators. The iterator `first` is incremented past the
   consumed bytes. */

template<class InputIt>
int32_t get_int32 (InputIt &first, const InputIt &last);

template<class InputIt>
uint32_t get_uint32 (InputIt &first, const InputIt &last);

template<class InputIt>
long double get_time (InputIt &first, const InputIt &last);

template<class InputIt>
float get_float (InputIt &first, const InputIt &last);

template<class InputIt>
std::string get_string (InputIt &first, const InputIt &last);

template<class InputIt>
std::string get_blob (InputIt &first, const InputIt &last);

#define OSC_MOD 4
#define NTP_DELTA 2208988800

/* Get OSC_MOD chars as type T from InputIt */
template<class T, class InputIt>
T get_from_c (InputIt &first, const InputIt &last) {
  // Get OSC_MOD bytes
  assert(last - first >= OSC_MOD);
  unsigned char bytes[OSC_MOD];
  copy_n(first, OSC_MOD, bytes);
  first += OSC_MOD;
  // Convert from big endian to system
  static_assert(sizeof(uint32_t) == OSC_MOD,
                "OSC packet size multiple must equal"
                "the size of 32 bit unsigned integer");
  uint32_t tmp;
  std::memcpy(&tmp, bytes, OSC_MOD);
  tmp = be32toh(tmp);
  // Copy to target type
  static_assert(std::is_trivially_copyable<T>(),
                "Target class must be a TriviallyCopyable type");
  static_assert(sizeof(T) == OSC_MOD,
                "Size of target class must equal OSC packet multiple");
  T val;
  std::memcpy(&val, &tmp, OSC_MOD);
  return val;
}

/* Get signed 32-bit integer from chars */
template<class InputIt>
int32_t get_int32 (InputIt &first, const InputIt &last) {
  return get_from_c<int32_t>(first, last);
}

/* Get unsigned 32-bit integer from chars */
template<class InputIt>
uint32_t get_uint32 (InputIt &first, const InputIt &last) {
  return get_from_c<uint32_t>(first, last);
}

/* Get timestamp from unsigned 64-bit NTP timestamp */
template<class InputIt>
long double get_time (InputIt &first, const InputIt &last) {
  const uint32_t secs = get_uint32(first, last);
  const uint32_t frac = get_uint32(first, last);
  // `long double` *should* be 80-bit float
  static_assert(std::numeric_limits<long double>::digits >=
                2*std::numeric_limits<uint32_t>::digits,
                "long double must have at least the number of digits"
                "of two 32 bit unsigned integers");
  long double t = secs + (long double) frac / (1ul << 32);
  t -= NTP_DELTA; // NTP time starts at 1 Jan 1900
  return t;
}

/* Get IEEE-754 32 bit float */
template<class InputIt>
float get_float (InputIt &first, const InputIt &last) {
  static_assert(std::numeric_limits<float>::is_iec559,
                "float implementation must conform to IEEE-754");
  return get_from_c<float>(first, last);
}

/* Get a \0 terminated and padded string */
template<class InputIt>
std::string get_string (InputIt &first, const InputIt &last) {
  InputIt end = std::find(first, last, '\0');
  const std::string str(first, end);
  end += ((first - end) % OSC_MOD) + OSC_MOD;
  assert(end <= last);
  first = end;
  return str;
}

/* Get arbitrary length binary blob */
template<class InputIt>
std::string get_blob (InputIt &first, const InputIt &last) {
  long count = get_int32(first, last);
  assert(last - first >= count);
  const std::string str(first, first + count);
  first += ((-count % OSC_MOD) + OSC_MOD) % OSC_MOD;
  assert(first <= last);
  return str;
}

#endif
