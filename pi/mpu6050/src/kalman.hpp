#pragma once

#include "imu.hpp"
#include "utility.hpp"

#include <eigen3/Eigen/Core>
#include <eigen3/Eigen/LU>

#include <iostream>

template<typename M>
M Q_continuous_white_noise(
    typename M::Scalar dt=1.,
    typename M::Scalar spectral_density=1.
) {
  static_assert(
      M::RowsAtCompileTime == M::ColsAtCompileTime &&			\
      (M::ColsAtCompileTime == 2 || M::ColsAtCompileTime == 3),		\
      "Matrix rows & cols must be equal and 2 or 3");
    M res;
    if(M::RowsAtCompileTime == 2) {
      res << pow(dt, 4) / 3, pow(dt, 2) / 2,
	     pow(dt, 2) / 2, dt;
    } else {
      res << pow(dt, 5) / 20, pow(dt, 4) / 8, pow(dt, 3) / 6,
             pow(dt, 4) / 8, pow(dt, 3) / 3,  pow(dt, 2) / 2,
             pow(dt, 3) / 6, pow(dt, 2) / 2,  dt;

    }
    return res * spectral_density;
};


template<typename M>
M Q_discrete_white_noise(
    typename M::Scalar dt=1.,
    typename M::Scalar sigma=1.
) {
  static_assert(
      M::RowsAtCompileTime == M::ColsAtCompileTime &&
      (M::ColsAtCompileTime == 2 || M::ColsAtCompileTime == 3),
      "Matrix rows & cols must be equal and 2 or 3");
    M res;
    if(M::RowsAtCompileTime == 2) {
      res << pow(dt, 4) * .25, pow(dt, 3) * .5,
	     pow(dt, 3) * .50, pow(dt, 2);
    } else {
      res << pow(dt, 4) * .25, pow(dt, 3) * .50, pow(dt, 2) * .50,
             pow(dt, 3) * .50, pow(dt, 2) * 1.0, dt              ,
             pow(dt, 2) * .50, dt              ,  1.0            ;

    }
    return res * sigma;
};


template<int StateDim, int MeasurementDim, int ControlDim, typename FloatT=double>
class KalmanFilter {
public:
  template<int Rows, int Cols>
  using MatrixT = Eigen::Matrix<FloatT, Rows, Cols>;

  using StateT = MatrixT<StateDim, 1>;
  using StateVarT = MatrixT<StateDim, StateDim>;

  using ControlT = MatrixT<ControlDim, 1>;
  using ControlFunctionT = MatrixT<StateDim, ControlDim>;

  using MeasurementT = MatrixT<MeasurementDim, 1>;
  using ProcessNoiseT = MatrixT<StateDim, StateDim>;
  using StateMeasurementT = MatrixT<MeasurementDim, StateDim>;
  using MeasurementNoiseT = MatrixT<MeasurementDim, MeasurementDim>;

  KalmanFilter()
    : x(decltype(x)::Zero())
    , P(decltype(P)::Zero())
    , Q(decltype(Q)::Zero())
    , B(decltype(B)::Zero())
    , F(decltype(F)::Zero())
    , H(decltype(H)::Zero())
    , R(decltype(R)::Zero())
    , updated(true)
  {
  }

  StateT x; // State
  StateVarT P; // State variance/uncertainty
  StateVarT Q; // Process noise
  ControlFunctionT B; // Control function
  StateVarT F; // State transition function


  StateMeasurementT H; // State-to-measurement function
  MeasurementNoiseT R; // Measurement noise

  void predict(ControlT u=ControlT::Zero()) {
    if(!updated) {
      x = x_;
      P = P_;
    }
    x_ = F * x + B * u;
    P_ = F * P * F.transpose() + Q;
    updated = false;
  }


  void update(const MeasurementT& z) {
    const auto S = H * P_ * H.transpose() + R;
    const auto K = P_ * H.transpose() * S.inverse();
    const auto y = z - H * x;
    const auto kalman_gain = K * y;
    x = x_ + kalman_gain;
    P = (decltype(P)::Identity() - K*H) * P_;
    updated = true;
    std::cout << "z: " << z << "\n";
    std::cout << "kalman_gain: " << kalman_gain << "\n";
    std::cout << "x: " << x << "\n";
  }


  void fake_update() {
      x = x_;
      P = P_;
      updated = true;
  }

private:
  decltype(x) x_;
  decltype(P) P_;

  bool updated;
};

template<int StateDim, int MeasurementDim, int ControlDim, typename FloatT>
std::ostream& operator<<(std::ostream& os, const KalmanFilter<StateDim, MeasurementDim, ControlDim, FloatT>& kf) {
  os << "x: \n" << kf.x << "\nP: \n" << kf.P << "\nQ: \n" << kf.Q << "\nF: \n" << kf.F << "\nH: \n" << kf.H << "\nR: \n" << kf.R;
  return os;
}


class IMUKalmanFilter : public TimeDeltaFilter<IMUKalmanFilter> {

public:
  using kalmanfilter_t = KalmanFilter<3, 3, 3>;
  using vector3_t = kalmanfilter_t::MeasurementT;

  IMUKalmanFilter(const std::string& jsonConfiguration);

  void filter(double dt, IMUData& result);

private:
  kalmanfilter_t _filter;

  bool _enabled;
};
