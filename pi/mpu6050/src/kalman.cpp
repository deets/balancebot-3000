#include "kalman.hpp"

IMUKalmanFilter::IMUKalmanFilter(const std::string& jsonConfiguration)
  : _enabled(true)
{
  _filter.x << 0, 0, 0;
  _filter.P = decltype(_filter.P)::Identity() * 500;
  _filter.F = decltype(_filter.F)::Identity();
  _filter.Q = decltype(_filter.Q)::Identity() * 2.0;

  _filter.B = decltype(_filter.B)::Identity();
  _filter.H = decltype(_filter.H)::Identity();
  _filter.R = decltype(_filter.R)::Identity() * 20.0;

  Json::Reader reader;
  Json::Value root;

  reader.parse(read(jsonConfiguration), root);
  assert(root.isObject());
  if(root.isMember("enabled")) {
    _enabled = root["enabled"].asBool();
  }
}


void IMUKalmanFilter::filter(double dt, IMUData& res) {
  Json::Value debugData(Json::objectValue);

  debugData["gyroXOriginal"] = res.gyroX;
  debugData["gyroYOriginal"] = res.gyroY;
  debugData["gyroZOriginal"] = res.gyroZ;

  const auto gx = deg2rad(res.gyroX) * dt;
  const auto gy = deg2rad(res.gyroY) * dt;
  const auto gz = deg2rad(res.gyroZ) * dt;

  const auto old_x = _filter.x;

  kalmanfilter_t::ControlT u;
  u << gx, gy, gz;
  _filter.predict(u);

  auto atanAccX = -atan2(-res.accY, res.accZ);
  auto atanAccY = atan2(-res.accX, res.accZ);
  auto atanAccZ = -atan2(-res.accX, res.accY);

  debugData["atanAccX"] = atanAccX;
  debugData["atanAccY"] = atanAccY;
  debugData["atanAccZ"] = atanAccZ;

  if(_enabled) {
     _filter.x(0) = radNorm(_filter.x(0));
     _filter.x(1) = radNorm(_filter.x(1));
     _filter.x(2) = radNorm(_filter.x(2));
     _filter.update(vector3_t(atanAccX, atanAccY, atanAccZ));
  } else {
    _filter.fake_update();
  }

  auto diff = _filter.x - old_x;
  res.gyroX = rad2deg(diff(0)) / dt;
  res.gyroY = rad2deg(diff(1)) / dt;
  res.gyroZ = rad2deg(diff(2)) / dt;

  debugData["gyroX"] = res.gyroX;
  debugData["gyroY"] = res.gyroY;
  debugData["gyroZ"] = res.gyroZ;

  res.jsonDebugData["kf"] = debugData;
}
