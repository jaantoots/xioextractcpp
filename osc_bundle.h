// -*- mode: c++ -*-
#ifndef OSC_BUNDLE_H
#define OSC_BUNDLE_H

#include <cassert>
#include <cstdint>
#include <cstdio>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "osc_message.h"
#include "osc_types.h"

namespace osc {

  /* OSC Bundle class that gets its content from iterators. Contains 0
     or more Elements, each a Message or another Bundle. Element class
     also captures the entire packet. */

  template<class InputIt>
  char element_type (const InputIt &first, const InputIt &last);

  class Element {
  public:
    template<class InputIt>
    Element (InputIt &first, const InputIt &last);
    const char type;
    std::shared_ptr<class Bundle> b;
    std::shared_ptr<Message> m;
  };

  class Bundle {
  public:
    template<class InputIt>
    Bundle (InputIt &first, const InputIt &last);
    void put_csv (FILE* dfp, const std::map<std::string, FILE*> files = {}) const;
  private:
    osc_time time;
    std::vector<Element> elem;
  };

  /* Exception for errors encountered during message parsing */
  struct BundleException : public std::exception {
    std::string msg;
    BundleException(std::string s) : msg(s) {}
    const char * what () const throw () { return msg.c_str(); }
  };

  /* Return element type as char */
  template<class InputIt>
  char element_type (const InputIt &first, const InputIt &last) {
    if (is_message(first, last))
      return 'm';
    return 'b';
  }

  /* Parse OSC Packet or Bundle Element */
  template<class InputIt>
  Element::Element (InputIt &first, const InputIt &last
                    ) : type(element_type(first, last)) {
    if (type == 'm') {
      m = std::make_shared<Message>(first, last);
    }
    if (type == 'b') {
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

}

#endif
