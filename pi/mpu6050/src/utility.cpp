#include "utility.hpp"

#include <math.h>
#include <chrono>
#include <fstream>
#include <cmath>


Json::Value imuToJson(const IMUData& sample, timestamp_t start_time) {
    Json::Value sampleEntry(Json::objectValue);

    sampleEntry["gyro"] = toJson(sample.gyro);
    sampleEntry["acc"] = toJson(sample.acc);
    sampleEntry["gyroAcc"] = toJson(sample.gyroAcc);
    sampleEntry["attitude"] = toJson(sample.attitude);
    sampleEntry["timestamp"] = ((sample.timestamp - start_time) / std::chrono::microseconds(1));

    sampleEntry["debugData"] = sample.jsonDebugData;
    return sampleEntry;
}



double radNorm(double r) {
  while(r > M_PI_2) {
    r -= M_PI;
  }
  while(r < -M_PI_2) {
    r += M_PI;
  }
  return r;
}

double circleNorm(double absolute, double relative) {
  if(absolute > relative && absolute - relative >= M_PI_2) {
    relative += M_PI * 2 * ceil((absolute - relative) / (M_PI * 2));
  } else if(absolute < relative && relative - absolute >= M_PI_2) {
    relative -= M_PI * 2 * ceil((relative - absolute) / (M_PI * 2));
  }
  return relative;
}

std::string read(const std::string& path) {
  std::ifstream inputFile(path);
  std::array<std::ifstream::char_type, 1024> buf;
  std::string res;
  while(!inputFile.eof() && inputFile.good()) {
    std::streamsize count = buf.size();
    auto bytesRead = inputFile.readsome(buf.data(), count);
    if(bytesRead == 0)
      break;
    std::copy(buf.data(), buf.data() + bytesRead, std::back_inserter(res));
  }
  return res;
}
