#include "kalman.hpp"

IMUKalmanFilter::IMUKalmanFilter(const std::string& jsonConfiguration)
  : _enabled(true)
  , _axisToFilter{
      AxisFilter::filterX,
      AxisFilter::filterY,
      AxisFilter::filterZ
    }
{
  _filter.x << 0, 0, 0;
  _filter.P = decltype(_filter.P)::Identity() * 10;
  _filter.F = decltype(_filter.F)::Identity();
  _filter.Q = decltype(_filter.Q)::Identity() * 2.0;

  _filter.B = decltype(_filter.B)::Identity();
  _filter.H = decltype(_filter.H)::Identity();
  _filter.R = decltype(_filter.R)::Identity() * 2000.0;

  Json::Reader reader;
  Json::Value root;

  reader.parse(read(jsonConfiguration), root);
  assert(root.isObject());
  if(root.isMember("enabled")) {
    _enabled = root["enabled"].asBool();
  }
  if(root.isMember("axisToFilter")) {
    _axisToFilter = std::set<AxisFilter>();
    for(const auto& jv : root["axisToFilter"]) {
      auto v = jv.asString();
      if(v == "x") {
	_axisToFilter.insert(AxisFilter::filterX);
      } else if(v == "y") {
	_axisToFilter.insert(AxisFilter::filterY);
      } else if(v == "z") {
	_axisToFilter.insert(AxisFilter::filterZ);
      }
    }
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

  debugData["atanAccX"] = rad2deg(atanAccX);
  debugData["atanAccY"] = rad2deg(atanAccY);
  debugData["atanAccZ"] = rad2deg(atanAccZ);

  if(_enabled) {
     _filter.x(0) = radNorm(_filter.x(0));
     _filter.x(1) = radNorm(_filter.x(1));
     _filter.x(2) = radNorm(_filter.x(2));
     auto update_vector = _filter.x;
     if(_axisToFilter.count(AxisFilter::filterX)) {
       update_vector[0] = atanAccX;
     }
     if(_axisToFilter.count(AxisFilter::filterY)) {
       update_vector[1] = atanAccY;
     }
     if(_axisToFilter.count(AxisFilter::filterZ)) {
       update_vector[2] = atanAccZ;
     }
     _filter.update(update_vector);
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

  res.gyroXAcc = rad2deg(_filter.x[0]);
  res.gyroYAcc = rad2deg(_filter.x[1]);
  res.gyroZAcc = rad2deg(_filter.x[2]);

  res.jsonDebugData["kf"] = debugData;
}
