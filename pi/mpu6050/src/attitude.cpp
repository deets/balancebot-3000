#include "attitude.hpp"
#include "utility.hpp"

#include <iostream>


AttitudeFilter::AttitudeFilter()
  : _gyroAcc(0, 0, 0)
{
}


void AttitudeFilter::filter(double dt, IMUData& res) {
  if(!_attitude) {
    const auto qx = deg2rad(res.gyroX) * dt * .5;
    const auto qy = deg2rad(res.gyroY) * dt * .5;
    const auto qz = deg2rad(res.gyroZ) * dt * .5;
    const auto qw = 1.0 - 0.5 * (qx * qx + qy * qy + qz * qz);
    _attitude = quaternion_t(qw, qx, qy, qz);
  } else {
    const auto qx = deg2rad(res.gyroX) * dt * .5;
    const auto qy = deg2rad(res.gyroY) * dt * .5;
    const auto qz = deg2rad(res.gyroZ) * dt * .5;
    const auto qw = 1.0 - 0.5 * (qx * qx + qy * qy + qz * qz);
    const auto deltaQ = quaternion_t(qw, qx, qy, qz);
    _attitude = deltaQ * (*_attitude);
  }
  res.attitude = *_attitude;
}
