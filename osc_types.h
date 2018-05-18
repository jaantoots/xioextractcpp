#ifndef OSC_TYPES_H
#define OSC_TYPES_H

#include <string>

template<class Iter>
int32_t get_int32 (Iter &first, const Iter &last);

template<class Iter>
uint32_t get_uint32 (Iter &first, const Iter &last);

template<class Iter>
long double get_time (Iter &first, const Iter &last);

template<class Iter>
float get_float (Iter &first, const Iter &last);

template<class Iter>
std::string get_string (Iter &first, const Iter &last);

template<class Iter>
std::string get_blob (Iter &first, const Iter &last);

#endif
