#pragma once

#include <Eigen/Geometry>
#include <chrono>
#include <json/json.h>

using timestamp_t = std::chrono::time_point<std::chrono::steady_clock>;
using quaternion_t = Eigen::Quaternion<double>;
using angleaxis_t = Eigen::AngleAxis<double>;
using vector3_t = Eigen::Matrix<double, 3, 1>;

struct IMUData {

  IMUData();

  double accX;
  double accY;
  double accZ;
  double gyroX;
  double gyroY;
  double gyroZ;

  double gyroXAcc;
  double gyroYAcc;
  double gyroZAcc;

  quaternion_t attitude;

  timestamp_t timestamp;
  double updateFreq;
  int    waitcnt;

  Json::Value jsonDebugData;
};
