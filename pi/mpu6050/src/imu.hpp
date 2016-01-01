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

  vector3_t acc;
  vector3_t gyro;
  vector3_t gyroAcc;

  quaternion_t attitude;

  timestamp_t timestamp;
  double updateFreq;
  int    waitcnt;

  Json::Value jsonDebugData;
};
