#include "attitude.hpp"
#include "utility.hpp"

AttitudeFilter::AttitudeFilter()
  : _gyroAcc(0, 0, 0)
{
  _attitude.setIdentity();
}


void AttitudeFilter::filter(double dt, IMUData& res) {
  const auto qx = deg2rad(res.gyroX) * dt * .5;
  const auto qy = deg2rad(res.gyroY) * dt * .5;
  const auto qz = deg2rad(res.gyroZ) * dt * .5;
  const auto qw = 1.0 - 0.5 * (qx * qx + qy * qy + qz * qz);
  const auto deltaQ = IMUData::quaternion_t(qw, qx, qy, qz);
  _attitude = deltaQ * _attitude;
  res.attitude = _attitude;
  _gyroAcc += vector3_t(res.gyroX, res.gyroY, res.gyroZ) * dt;
  res.gyroXAcc = _gyroAcc[0];
  res.gyroYAcc = _gyroAcc[1];
  res.gyroZAcc = _gyroAcc[2];
}
