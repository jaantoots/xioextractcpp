#include "osc_bundle.h"

namespace osc {

  /* Print bundle contents as csv to file streams */
  void Bundle::put_csv (FILE* dfp,
                        const std::map<std::string, FILE*> files) const {
    for (const Element el : elem) {
      if (el.type == 'm') {
        FILE* fp = files.count(el.m->address) ? files.at(el.m->address) : dfp;
        fprintf(fp, "%.11Lf", time);
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
