#pragma once

#include "imu.hpp"
#include <json/json.h>

#include <boost/optional.hpp>


Json::Value imuToJson(const IMUData& sample, timestamp_t start_time);

template<typename>
struct json_converter;

template<typename T>
Json::Value toJson(const T&);

template<typename T>
T deg2rad(const T&);
template<typename T>
T rad2deg(const T&);

double radNorm(double);
/* Norms the relative rotation given in rad
 * to be closest to the given absolute.
 *
 * e.g: absolute = 2*PI
 *      relative = PI/4
 *      result   = 2*PI + PI/4
 */
double circleNorm(double absolute, double relative);

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


template<typename T>
T deg2rad(const T& deg) {
  return (T)(deg * (M_PI / 180.0));
}


template<typename T>
T rad2deg(const T& rad) {
  return (T)(rad * (180.0 / M_PI));
}

#include "utility.ipp"
