#include <cassert>
#include <cstdio>
#include <string>

#include "osc_bundle.h"
#include "osc_message.h"

namespace osc {

  /* Print element contents as csv to file stream */
  void Element::put_csv (FILE* fp) const {
    switch(type) {
    case 'm' : m->put_csv(fp);
      break;
    case 'b' : b->put_csv(fp);
      break;
    default : assert(!"bad element type");
    }
  }

  /* Print element bundle as csv to file streams */
  void Element::putmsg_csv (const std::map<std::string, FILE*> files) const {
    assert(type == 'b');
    b->putmsg_csv(files);
  }

  /* Print bundle contents as csv to file stream */
  void Bundle::put_csv (FILE* fp) const {
    fprintf(fp, "%.11Lf", time);
    for (const Element el : elem) {
      fprintf(fp, ",");
      el.put_csv(fp);
    }
  }

  /* Print bundle messages as csv to file streams */
  void Bundle::putmsg_csv (const std::map<std::string, FILE*> files) const {
    for (const Element el : elem) {
      if (el.type == 'm') {
        FILE* fp = files.at(el.m->address);
        fprintf(fp, "%.11Lf", time);
        el.m->putargs_csv(fp);
        fprintf(fp, "\n");
      }
      else
        el.b->putmsg_csv(files);
    }
  }

}
