#include <cassert>
#include <cstdio>

#include "osc_message.h"

namespace osc {

  /* Print message contents as csv to file stream */
  void Message::put_csv (FILE* fp) const {
    fprintf(fp, "%s", address.c_str());
    putargs_csv(fp);
    fprintf(fp, "\n");
  }

  /* Print message arguments as csv to file stream */
  void Message::putargs_csv (FILE* fp) const {
    for (const Argument arg : args) {
      switch(arg.type) {
      case 'i' : fprintf(fp, ",%d", arg.i);
        break;
      case 'f' : fprintf(fp, ",%.11g", arg.f);
        break;
      case 's' : fprintf(fp, ",%s", arg.s.c_str());
        break;
      case 'b' : fprintf(fp, ",0x");
        for (unsigned char c : arg.b)
          fprintf(fp, "%.2hhx", c);
        break;
      case 't' : fprintf(fp, ",%.11Lf", arg.t);
        break;
      case 'T' :
      case 'F' :
        fprintf(fp, ",%s", arg.o ? "True" : "False");
        break;
      default : assert(!"type tag printing not implemented");
      }
    }
  }

}
