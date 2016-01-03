#include "kalman.hpp"

template<int A, int B, int C, typename FloatT>
struct json_converter<KalmanFilter<A, B, C, FloatT>> {

  static Json::Value toJson(const KalmanFilter<A, B, C, FloatT>& kf) {
    auto res = Json::Value(Json::objectValue);
    res["x"] = ::toJson(kf.x);
    res["P"] = ::toJson(kf.P);
    return res;
  }
};


IMUKalmanFilter::IMUKalmanFilter(const std::string& jsonConfiguration)
  : _enabled(true)
  , _axisToFilter{
      AxisFilter::filterX,
      AxisFilter::filterY,
      AxisFilter::filterZ
    },
    _accLengthThreshold{0.0},
    _atanThreshold{0.0}
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

  if(root.isMember("accLengthThreshold")) {
    _accLengthThreshold = root["accLengthThreshold"].asDouble();
  }
  if(root.isMember("atanThreshold")) {
    _atanThreshold = root["atanThreshold"].asDouble();
  }

}


void IMUKalmanFilter::filter(double dt, IMUData& res) {
  auto shouldFilterForAxis = [this](const double A, const double B, AxisFilter axis) -> bool {
    auto res = _axisToFilter.count(axis) > 0 &&
    (_atanThreshold == 0.0 || (A*A + B*B) > _atanThreshold * _atanThreshold)
    ;
    //std::cout << "axis " << axis << " filtered: " << res << "\n";
    return res;
  };

  auto accVectorWithinThreshold = [this, &res]() {
    return _accLengthThreshold == 0.0 || std::abs(1.0 - res.acc.norm()) < _accLengthThreshold;
  };

  Json::Value debugData(Json::objectValue);
  const auto old_x = _filter.x;

  debugData["gyroXOriginal"] = res.gyro(0);
  debugData["gyroYOriginal"] = res.gyro(1);
  debugData["gyroZOriginal"] = res.gyro(2);

  _filter.predict(deg2rad(res.gyro) * dt);

  vector3_t atanAcc = _filter.x;

  if(shouldFilterForAxis(res.acc(1), res.acc(2), AxisFilter::filterX)) {
    atanAcc(0) = circleNorm(_filter.x(0), -atan2(-res.acc(1), res.acc(2)));
  }
  if(shouldFilterForAxis(res.acc(0), res.acc(2), AxisFilter::filterY)) {
    atanAcc(1) = circleNorm(_filter.x(1), atan2(-res.acc(0), res.acc(2)));
  }
  if(shouldFilterForAxis(res.acc(0), res.acc(1), AxisFilter::filterZ)) {
    atanAcc(2) = circleNorm(_filter.x(2), -atan2(-res.acc(0), res.acc(1)));
  }

  debugData["atanAccX"] = atanAcc(0);
  debugData["atanAccY"] = atanAcc(1);
  debugData["atanAccZ"] = atanAcc(2);
  debugData["atanAccXRaw"] = -atan2(-res.acc(1), res.acc(2));
  debugData["atanAccYRaw"] = atan2(-res.acc(0), res.acc(2));
  debugData["atanAccZRaw"] = -atan2(-res.acc(0), res.acc(1));
  debugData["accOutsideThreshold"] = accVectorWithinThreshold() ? 0 : 1;

  if(_enabled && accVectorWithinThreshold()) {
      _filter.update(atanAcc);
    } else {
      _filter.nop_update();
    }

  vector3_t diff = _filter.x - old_x;
  res.gyro = rad2deg(diff) / dt;

  debugData["gyroX"] = res.gyro(0);
  debugData["gyroY"] = res.gyro(1);
  debugData["gyroZ"] = res.gyro(2);
  debugData["state"] = toJson(_filter);

  res.gyroAcc = rad2deg(_filter.x);
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
