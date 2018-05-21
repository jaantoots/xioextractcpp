#include <cstdio>
#include <cstdlib>
#include <map>
#include <string>
#include <vector>
#include <sys/stat.h>

#include "slip.h"
#include "osc_bundle.h"

const std::vector<std::string> addresses = { "/altitude", "/battery", "/button", "/earth", "/euler", "/humidity", "/linear", "/magnitudes", "/matrix", "/quaternion", "/sensors", "/temperature", "/settings" };

const std::map<std::string, std::string> headers = {
  { "/altitude", "Time (s),Altitude (m)" },
  { "/battery", "Time (s),Percentage (%),Time To Empty (minutes),Voltage (V),Current (mA)" },
  { "/button", "Time (s)" },
  { "/earth", "Time (s),X (g),Y (g),Z (g)" },
  { "/euler", "Time (s),Roll (deg),Pitch (deg),Yaw (deg)" },
  { "/humidity", "Time (s),Humidity (%)" },
  { "/linear", "Time (s),X (g),Y (g),Z (g)" },
  { "/magnitudes", "Time (s),Gyroscope (deg/s),Accelerometer (g),Magnetometer (uT)" },
  { "/matrix", "Time (s),XX,XY,XZ,YX,YY,YZ,ZX,ZY,ZZ" },
  { "/quaternion", "Time (s),W,X,Y,Z" },
  { "/sensors", "Time (s),Gyroscope X (deg/s),Gyroscope Y (deg/s),Gyroscope Z (deg/s),Accelerometer X (g),Accelerometer Y (g),Accelerometer Z (g),Magnetometer X (uT),Magnetometer Y (uT),Magnetometer Z (uT),Barometer (hPa)" },
  { "/temperature", "Time (s),Processor (degC),Gyroscope And Accelerometer (degC),Environmental Sensor (degC)" }
};

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
  for (std::string addr : addresses) {
    files[addr] = file_open(dir + "/" + addr.substr(1) + ".csv", "w");
    if (headers.count(addr))
      fprintf(files[addr], "%s\n", headers.at(addr).c_str());
  }
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
          for (std::string addr : addresses) {
            files[addr] = file_open(dir + "/" + addr.substr(1) + ".csv", "w");
            if (headers.count(addr))
              fprintf(files[addr], "%s\n", headers.at(addr).c_str());
          }
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
