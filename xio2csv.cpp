#include <cstdio>
#include <cstdlib>
#include <map>
#include <string>
#include <vector>
#include <sys/stat.h>

#include "slip.h"
#include "osc_bundle.h"

const std::vector<std::string> addresses = { "/altitude", "/battery", "/button", "/earth", "/euler", "/humidity", "/linear", "/magnitudes", "/matrix", "/quaternion", "/sensors", "/temperature", "/settings" };

void mkdir_str (std::string dir) {
  if (mkdir((dir).c_str(), S_IRWXU | S_IRWXG | S_IRWXO) != 0 &&
      errno != EEXIST) {
    perror((dir).c_str());
    std::exit(EXIT_FAILURE);
  }
}

FILE* file_open (const std::string file, const char *mode) {
  FILE* fp = fopen(file.c_str(), mode);
  if(!fp) {
    perror(file.c_str());
    std::exit(EXIT_FAILURE);
  }
  return fp;
}

void file_close (FILE* fp) {
  if (ferror(fp))
    puts("warning: I/O error");
  fclose(fp);
}

int main (int argc, char *argv[]) {
  std::string inf;
  std::string outd;
  switch (argc) {
  case 3 :
    inf = argv[1];
    outd = argv[2];
    break;
  case 2 :
    inf = argv[1];
    outd = inf.substr(0, inf.find_last_of('.'));
    break;
  default : printf("usage: %s FILE [DIR]\n", argv[0]);
    return 1;
  }

  FILE* fp = file_open(inf, "r");

  mkdir_str(outd);
  if (outd.back() != '/')
    outd += '/';

  int i = 0;
  std::string dir = outd + std::to_string(i);
  mkdir_str(dir);
  unsigned long bundles = 0;
  std::map<std::string, FILE*> files;
  for (std::string addr : addresses)
    files[addr] = file_open(dir + "/" + addr.substr(1) + ".csv", "w");
  while (!(feof(fp))) {
    std::vector<unsigned char> dgram = slip::get_dgram(fp);
    if (!(dgram.empty())) {
      auto it = dgram.begin();
      osc::Element elem = osc::Element(it, dgram.end());
      switch (elem.type) {
      case 'm' :
        if (bundles != 0) {
          for (auto file : files)
            file_close(file.second);
          ++i;
          dir = outd + std::to_string(i);
          mkdir_str(dir);
          bundles = 0;
          for (std::string addr : addresses)
            files[addr] = file_open(dir + "/" + addr.substr(1) + ".csv", "w");
        }
        elem.put_csv(files["/settings"]);
        break;
      case 'b' :
        ++bundles;
        elem.putmsg_csv(files);
        break;
      default :
        assert(!"unknown element type");
      }
    }
  }
  for (auto file : files)
    file_close(file.second);

  file_close(fp);
  return 0;
}
