#pragma once

#include "imu.hpp"

#include <json/json.h>

#include <string>

class IMUCalibration {

public:

  static IMUCalibration fromJsonFile(const std::string& path);
  static IMUCalibration fromJson(const Json::Value& calibration);

  IMUCalibration();
  IMUCalibration(const IMUData& calibrationOffsets);

  IMUData operator()(const IMUData& input);

private:

  IMUData _calibrationOffsets;
};
