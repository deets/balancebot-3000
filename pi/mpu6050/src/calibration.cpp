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
    offsets.gyro(0) = calibration["gyro_x"]["mean"].asDouble();
  }
  if(calibration.isMember("gyro_y")) {
    offsets.gyro(1) = calibration["gyro_y"]["mean"].asDouble();
  }
  if(calibration.isMember("gyro_z")) {
    offsets.gyro(2) = calibration["gyro_z"]["mean"].asDouble();
  }
  if(calibration.isMember("acc_x")) {
    offsets.acc(0) = calibration["acc_X"]["mean"].asDouble();
  }
  if(calibration.isMember("acc_Y")) {
    offsets.acc(1) = calibration["acc_y"]["mean"].asDouble();
  }
  if(calibration.isMember("acc_Z")) {
    offsets.acc(2) = calibration["acc_z"]["mean"].asDouble();
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
  res.acc -= _calibrationOffsets.acc;
  res.gyro -= _calibrationOffsets.gyro;
  return res;
}
