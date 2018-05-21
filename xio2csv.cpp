#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <string>
#include <vector>
#include <sys/stat.h>

#include "slip.h"
#include "osc_bundle.h"

const std::vector<std::string> addresses = {
  "/altitude", "/battery", "/button", "/earth", "/euler", "/humidity",
  "/linear", "/magnitudes", "/matrix", "/quaternion", "/sensors",
  "/temperature", "/settings" };

#define H_TIME "Time (s)"
#define H_XYZG "X (g),Y (g),Z (g)"
#define H_XYZ(name,unit)                                                \
  name " X (" unit ")," name " Y (" unit ")," name " Z (" unit ")"

const std::map<std::string, std::string> headers = {
  { "/altitude", H_TIME ",Altitude (m)" },
  { "/battery", H_TIME ",Percentage (%),Time To Empty (minutes),Voltage (V),Current (mA)" },
  { "/button", H_TIME },
  { "/earth", H_TIME "," H_XYZG },
  { "/euler", H_TIME ",Roll (deg),Pitch (deg),Yaw (deg)" },
  { "/humidity", H_TIME ",Humidity (%)" },
  { "/linear", H_TIME "," H_XYZG },
  { "/magnitudes", H_TIME ",Gyroscope (deg/s),Accelerometer (g),Magnetometer (uT)" },
  { "/matrix", H_TIME ",XX,XY,XZ,YX,YY,YZ,ZX,ZY,ZZ" },
  { "/quaternion", H_TIME ",W,X,Y,Z" },
  { "/sensors", H_TIME "," H_XYZ("Gyroscope", "deg/s") "," H_XYZ("Accelerometer", "g") "," H_XYZ("Magnetometer", "uT") ",Barometer (hPa)" },
  { "/temperature", H_TIME ",Processor (degC),Gyroscope And Accelerometer (degC),Environmental Sensor (degC)" }
};

#undef H_TIME
#undef H_XYZG
#undef H_XYZ

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

void init_files (const std::string &dir, std::map<std::string, FILE*> &files) {
  for (std::string addr : addresses) {
    files[addr] = file_open(dir + "/" + addr.substr(1) + ".csv", "w");
    if (headers.count(addr))
      fprintf(files[addr], "%s\n", headers.at(addr).c_str());
  }
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
  init_files(dir, files);
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
          init_files(dir, files);
        }
        elem.m->put_csv(files["/settings"]);
        break;
      case 'b' :
        ++bundles;
        elem.b->put_csv(stdout, files);
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
