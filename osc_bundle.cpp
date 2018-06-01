#include <cmath>
#include "osc_bundle.h"

namespace osc {

  /* Print bundle contents as csv to file streams */
  void Bundle::put_csv (FILE* dfp,
                        const std::map<std::string, FILE*> files) const {
    for (const Element el : elem) {
      if (el.type == 'm') {
        FILE* fp = files.count(el.m->address) ? files.at(el.m->address) : dfp;
        if (time.is_double)
          fprintf(fp, "%.11Lf", time.f);
        else
          fprintf(fp, "%ld.%011ld", (unsigned long) (time.d >> 32),
                  std::lround((double) (time.d % (1ul << 32))
                              / (1ul <<32) * 1e11));
        if (files.count(el.m->address)) {
          el.m->putargs_csv(fp);
          fprintf(fp, "\n");
        }
        else {
          fprintf(fp, ",");
          el.m->put_csv(fp);
        }
      }
      else
        el.b->put_csv(dfp, files);
    }
  }

}
