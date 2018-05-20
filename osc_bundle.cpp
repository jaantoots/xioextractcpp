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

int main (int argc, char *argv[]) {
  std::string in("#bundle\0\0\0\0\0\0\0\0\1\0\0\0\x28/addr\0\0\0,stb\0\0\0\0bundle\0\0\xff\xff\xff\xff\xff\xff\xff\xff\0\0\0\2\x0f\x00\0\0\0\0\0\x3c#bundle\0\0\0\0\0\0\0\0\1\0\0\0\x28/addr\0\0\0,stb\0\0\0\0bundle\0\0\xff\xff\xff\xff\xff\xff\xff\xff\0\0\0\2\x0f\x00\0\0", 124);
  std::string::iterator it = in.begin();
  osc::Bundle b = osc::Bundle(it, in.end());
  b.put_csv(stdout);
  printf("\n");

  std::string is("/addr\0\0\0,stb\0\0\0\0bundle\0\0\xff\xff\xff\xff\xff\xff\xff\xff\0\0\0\2\x0f\x00\0\0", 40);
  std::string::iterator itt = is.begin();
  osc::Message msg = osc::Message(itt, is.end());
  msg.put_csv(stdout);
  printf("\n");
  return 0;
}
