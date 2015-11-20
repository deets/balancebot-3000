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
  if(root.isMember("accStdDev")) {
    _accDeviation = vector3_t(
	root["accStdDev"][0].asDouble(),
	root["accStdDev"][1].asDouble(),
	root["accStdDev"][2].asDouble()
    );
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

  printf("-accY: %f accZ: %f accDeviation.y %f accDeviation.z %f \n",
	 -res.accY, res.accZ, _accDeviation(1),  _accDeviation(2));

  auto threshold = 0.2 * 0.2;

  auto shouldFilterForAxis = [threshold, this](const double A, const double B, AxisFilter axis) -> bool {
    auto res = _axisToFilter.count(axis) > 0 && (A*A + B*B) > threshold;
    std::cout << "axis " << axis << " filtered: " << res << "\n";
    return res;
  };

  double atanAccX, atanAccY, atanAccZ;

  if(shouldFilterForAxis(res.accY, res.accZ, AxisFilter::filterX)) {
    atanAccX = circleNorm(_filter.x(0), -atan2(-res.accY, res.accZ));
  } else {
    atanAccX = _filter.x(0);
  }
  if(shouldFilterForAxis(res.accX, res.accZ, AxisFilter::filterY)) {
    atanAccY = circleNorm(_filter.x(1), atan2(-res.accX, res.accZ));
  } else {
    atanAccY = _filter.x(1);
  }
  if(shouldFilterForAxis(res.accX, res.accY, AxisFilter::filterZ)) {
    atanAccZ = circleNorm(_filter.x(2), -atan2(-res.accX, res.accY));
  } else {
    atanAccZ = _filter.x(2);
  }
  debugData["atanAccX"] = rad2deg(atanAccX);
  debugData["atanAccY"] = rad2deg(atanAccY);
  debugData["atanAccZ"] = rad2deg(atanAccZ);

  if(_enabled) {
     auto update_vector = vector3_t(atanAccX, atanAccY, atanAccZ);
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

std::ostream& operator<<(std::ostream& os, const IMUKalmanFilter::AxisFilter& axis) {
  switch(axis) {
    case IMUKalmanFilter::AxisFilter::filterX:
      os << "filterX";
      break;
    case IMUKalmanFilter::AxisFilter::filterY:
      os << "filterY";
      break;
    case IMUKalmanFilter::AxisFilter::filterZ:
      os << "filterZ";
      break;
  }
  return os;
}
