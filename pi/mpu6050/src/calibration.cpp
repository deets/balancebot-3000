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

  if(calibration.isMember("gyro_x")) {
    offsets.gyroX = calibration["gyro_x"]["mean"].asDouble();
  }
  if(calibration.isMember("gyro_y")) {
    offsets.gyroY = calibration["gyro_y"]["mean"].asDouble();
  }
  if(calibration.isMember("gyro_z")) {
    offsets.gyroZ = calibration["gyro_z"]["mean"].asDouble();
  }
  if(calibration.isMember("acc_x")) {
    offsets.accX = calibration["acc_X"]["mean"].asDouble();
  }
  if(calibration.isMember("acc_Y")) {
    offsets.accY = calibration["acc_y"]["mean"].asDouble();
  }
  if(calibration.isMember("acc_Z")) {
    offsets.accZ = calibration["acc_z"]["mean"].asDouble();
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
