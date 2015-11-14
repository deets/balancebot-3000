#pragma once

#include "imu.hpp"
#include "utility.hpp"

#include <eigen3/Eigen/Core>

class AttitudeFilter : public TimeDeltaFilter<AttitudeFilter> {

public:
  using vector3_t = Eigen::Matrix<double, 3, 1>;

  AttitudeFilter();

  void filter(double dt, IMUData& result);

private:
  vector3_t _gyroAcc;
  quaternion_t _attitude;
};
