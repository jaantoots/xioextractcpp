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

  /* Print bundle contents as csv to file stream */
  void Bundle::put_csv (FILE* fp) const {
    fprintf(fp, "%.11Lf", time);
    for (const Element el : elem) {
      fprintf(fp, ",");
      el.put_csv(fp);
    }
  }

}
