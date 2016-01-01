#include "attitude.hpp"
#include "utility.hpp"

#include <iostream>


AttitudeFilter::AttitudeFilter()
  : _gyroAcc(0, 0, 0)
{
}


void AttitudeFilter::filter(double dt, IMUData& res) {
  if(!_attitude) {
    const auto q = deg2rad(res.gyro) * dt * .5;
    const auto qw = 1.0 - 0.5 * (q(0) * q(0) + q(1) * q(1) + q(2) * q(2));
    _attitude = quaternion_t(qw, q(0), q(1), q(2));
  } else {
    const auto q = deg2rad(res.gyro) * dt * .5;
    const auto qw = 1.0 - 0.5 * (q(0) * q(0) + q(1) * q(1) + q(2) * q(2));
    const auto deltaQ = quaternion_t(qw, q(0), q(1), q(2));
    _attitude = deltaQ * (*_attitude);
  }
  res.attitude = *_attitude;
}
