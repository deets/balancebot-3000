#include "calibration.hpp"

#include <fstream>
#include <cassert>
#include <cstring>


IMUCalibration IMUCalibration::fromJsonFile(const std::string& path) {
  Json::Value calibration;
  std::ifstream inf(path);
  inf >> calibration;
  return fromJson(calibration);
}


IMUCalibration IMUCalibration::fromJson(const Json::Value& calibration) {
  IMUData offsets;

  if(calibration.isMember("gyroX")) {
    offsets.gyroX = calibration["gyroX"].asDouble();
  }
  if(calibration.isMember("gyroY")) {
    offsets.gyroY = calibration["gyroY"].asDouble();
  }
  if(calibration.isMember("gyroZ")) {
    offsets.gyroZ = calibration["gyroZ"].asDouble();
  }
  if(calibration.isMember("accX")) {
    offsets.accX = calibration["accX"].asDouble();
  }
  if(calibration.isMember("accY")) {
    offsets.accY = calibration["accY"].asDouble();
  }
  if(calibration.isMember("accZ")) {
    offsets.accZ = calibration["accZ"].asDouble();
  }

  return IMUCalibration(offsets);
}


IMUCalibration::IMUCalibration()
{
  std::memset(&_calibrationOffsets, 0, sizeof(IMUData));
}


IMUCalibration::IMUCalibration(const IMUData& calibrationOffsets)
  : _calibrationOffsets(calibrationOffsets)
{
}


IMUData IMUCalibration::operator()(const IMUData& input) {
  IMUData res = input;
  res.accX -= _calibrationOffsets.accX;
  res.accY -= _calibrationOffsets.accY;
  res.accZ -= _calibrationOffsets.accZ;
  res.gyroX -= _calibrationOffsets.gyroX;
  res.gyroY -= _calibrationOffsets.gyroY;
  res.gyroZ -= _calibrationOffsets.gyroZ;
  return res;
}
