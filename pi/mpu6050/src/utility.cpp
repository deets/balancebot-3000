#include "utility.hpp"

#include <math.h>
#include <chrono>
#include <fstream>


namespace {
  template<typename T>
  Json::Value toJson(const T&) {
    return Json::Value(Json::objectValue);
  }

  template<>
  Json::Value toJson<quaternion_t>(const quaternion_t& quat) {
    Json::Value quatJ(Json::arrayValue);
    quatJ.resize(4);
    quatJ[0] = quat.w();
    quatJ[1] = quat.x();
    quatJ[2] = quat.y();
    quatJ[3] = quat.z();
    return quatJ;
  }
};


Json::Value imuToJson(const IMUData& sample, timestamp_t start_time) {
    Json::Value sampleEntry(Json::objectValue);
    Json::Value gyroEntry(Json::arrayValue);
    gyroEntry.resize(3);
    Json::Value accEntry(Json::arrayValue);
    accEntry.resize(3);
    Json::Value gyroAccEntry(Json::arrayValue);
    gyroAccEntry.resize(3);

    gyroEntry[0] = sample.gyroX;
    gyroEntry[1] = sample.gyroY;
    gyroEntry[2] = sample.gyroZ;

    accEntry[0] = sample.accX;
    accEntry[1] = sample.accY;
    accEntry[2] = sample.accZ;

    gyroAccEntry[0] = sample.gyroXAcc;
    gyroAccEntry[1] = sample.gyroYAcc;
    gyroAccEntry[2] = sample.gyroZAcc;

    sampleEntry["gyro"] = gyroEntry;
    sampleEntry["acc"] = accEntry;
    sampleEntry["gyroAcc"] = gyroAccEntry;
    sampleEntry["attitude"] = toJson(sample.attitude);
    sampleEntry["timestamp"] = ((sample.timestamp - start_time) / std::chrono::microseconds(1));

    sampleEntry["debugData"] = sample.jsonDebugData;
    return sampleEntry;
}


double deg2rad(double deg) {
  return deg * M_PI / 180.0;
}


double rad2deg(double rad) {
  return rad * 180.0 / M_PI;
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
