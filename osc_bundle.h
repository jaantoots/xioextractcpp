// -*- mode: c++ -*-
#ifndef OSC_BUNDLE_H
#define OSC_BUNDLE_H

#include <cassert>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "osc_message.h"
#include "osc_types.h"

class Element {
public:
  template<class InputIt>
  Element (InputIt &first, const InputIt &last);
  void put_csv (FILE* fp) const;
private:
  char type;
  std::shared_ptr<class Bundle> b;
  std::shared_ptr<Message> m;
};

class Bundle {
public:
  template<class InputIt>
  Bundle (InputIt &first, const InputIt &last);
  void put_csv (FILE* fp) const;
private:
  long double time;
  std::vector<Element> elem;
};

/* Exception for errors encountered during message parsing */
struct BundleException : public std::exception {
  std::string msg;
  BundleException(std::string s) : msg(s) {}
  const char * what () const throw () { return msg.c_str(); }
};

/* Parse OSC Packet or Bundle Element */
template<class InputIt>
Element::Element (InputIt &first, const InputIt &last) {
  if (is_message(first, last)) {
    type = 'm';
    m = std::make_shared<Message>(first, last);
  }
  else {
    type = 'b';
    b = std::make_shared<Bundle>(first, last);
  }
  assert(first == last);
}

/* Construct Bundle object from iterators */
template<class InputIt>
Bundle::Bundle (InputIt &first, const InputIt &last) {
  // OSC String "#bundle" begins bundle
  std::string b = get_string(first, last);
  if (b != "#bundle")
    throw BundleException("Invalid OSC Bundle");
  // OSC Time Tag is next
  time = get_time(first, last);
  // Parse OSC Bundle Elements
  while (first < last) {
    int32_t size = get_int32(first, last);
    const InputIt end = first + size;
    elem.push_back(Element(first, end));
  }
  assert(first == last);
}

#endif
