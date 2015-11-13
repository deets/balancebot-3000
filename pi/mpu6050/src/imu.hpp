#pragma once

#include <Eigen/Geometry>
#include <chrono>
#include <json/json.h>


struct IMUData {
  using timestamp_t = std::chrono::time_point<std::chrono::steady_clock>;
  using quaternion_t = Eigen::Quaternion<double>;

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
