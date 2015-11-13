#include "imu.hpp"

#include <cstring>

IMUData::IMUData()
  : jsonDebugData(Json::objectValue)
{
  std::memset(this, 0, sizeof(IMUData));
  attitude.setIdentity();
}
