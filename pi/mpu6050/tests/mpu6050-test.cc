#include "utility.hpp"
#include "gtest/gtest.h"

#include <math.h>


TEST(MPU6050Tests, CircleNormA) {
  ASSERT_EQ(
      M_PI * 2 + M_PI / 4, 
      circleNorm(M_PI * 2, M_PI / 4)
  );
}

TEST(MPU6050Tests, CircleNormB) {
  ASSERT_EQ(
      -M_PI * 2 - M_PI / 4, 
      circleNorm(-M_PI * 2, -M_PI / 4)
  );
}

TEST(MPU6050Tests, CircleNormC) {
  ASSERT_EQ(
      M_PI / 4, 
      circleNorm(0, M_PI / 4)
  );
}

TEST(MPU6050Tests, CircleNormD) {
  ASSERT_EQ(
      -M_PI / 4, 
      circleNorm(0, -M_PI / 4)
  );
}


TEST(MPU6050Tests, CircleNormCrossOver180Degree) {
  auto gyroX = deg2rad(200.0);
  auto accAtanX = deg2rad(-180.0 + 20.0);
  ASSERT_FLOAT_EQ(gyroX, circleNorm(gyroX, accAtanX));
}


TEST(MPU6050Tests, CircleNormCrossOverMinus180Degree) {
  auto gyroX = deg2rad(-200.0);
  auto accAtanX = deg2rad(180.0 - 20.0);
  ASSERT_FLOAT_EQ(gyroX, circleNorm(gyroX, accAtanX));
}


TEST(MPU6050Tests, TurnLeft) {
  for(int i=0; i < 720; i++) {
    auto gyroX = deg2rad(static_cast<double>(i));
    auto accAtanX = atan2(sin(gyroX), cos(gyroX));
    auto cn = circleNorm(gyroX, accAtanX);
    std::cout << "gyroX: " << gyroX << " accAtanX: " << accAtanX << " cn: " << cn << "\n";
    ASSERT_FLOAT_EQ(gyroX, cn);
  }
}

TEST(MPU6050Tests, TurnRight) {
  for(int i=0; i > -720; i--) {
    auto gyroX = deg2rad(static_cast<double>(i));
    auto accAtanX = atan2(sin(gyroX), cos(gyroX));
    auto cn = circleNorm(gyroX, accAtanX);
    std::cout << "gyroX: " << gyroX << " accAtanX: " << accAtanX << " cn: " << cn << "\n";
    ASSERT_FLOAT_EQ(gyroX, cn);
  }
}



