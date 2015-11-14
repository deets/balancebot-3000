#pragma once

#include "imu.hpp"
#include <json/json.h>

#include <boost/optional.hpp>


Json::Value imuToJson(const IMUData& sample, timestamp_t start_time);

double deg2rad(double);
double rad2deg(double);
double radNorm(double);

std::string read(const std::string& path);

template<typename T>
class TimeDeltaFilter {

public:
  TimeDeltaFilter()
  {
  }

  IMUData operator()(const IMUData& input) {
    IMUData res = input;
    if(_lastTimestamp) {
      auto nanoseconds = (input.timestamp - _lastTimestamp.get()) \
	/ std::chrono::nanoseconds(1);
      const double dt = static_cast<double>(nanoseconds) / 1000000000.0;
      static_cast<T*>(this)->filter(dt, res);
    }
    _lastTimestamp = input.timestamp;
    return res;
  }

private:
  boost::optional<timestamp_t> _lastTimestamp;

};
