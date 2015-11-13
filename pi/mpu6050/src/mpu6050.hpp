#pragma once

/**

The MPU6050 is a cheap IMU that can be obtained on expansion boards
for a few Euro.

It has 6DOF and is connected and worked with via I2C.

The Accelerometers and Gyros are aligned in a right-handed coordinate
system, like this:



     Z
     |	  Y
     |	 /
     | 	/
     | /
     |/
     +-------X
    O

The package of the chip looks like this:


   Y
   +---------+
   |o  	     |
   |	     |
   |	     |
   +---------+ X

with the Z-Axis pointing upwards.

Rotation is measured clockwise if you look from the origin O to the
axis X, Y, Z

So e.g. tilting the device around the X-axis making the Y-axis point
upward should result in positive X-gyro readings.


*/

#include "i2c.hpp"
#include "imu.hpp"

#include <functional>
#include <atomic>
#include <thread>

class MPU6050 {
public:

  enum class I2C_ADDRESS {
    DEFAULT = 0x68
  };

  enum class GYRO_SENSITIVITY {
    DEG250PS = 0,
    DEG500PS = 1,
    DEG1000PS = 2,
    DEG2000PS = 3
  };

  MPU6050(
      I2CBus& _bus,
      GYRO_SENSITIVITY gs=GYRO_SENSITIVITY::DEG250PS,
      I2C_ADDRESS address=I2C_ADDRESS::DEFAULT
  );
  virtual ~MPU6050();

  IMUData sample(I2CBus& bus, bool waitForData=true);

private:
  double _accSensitivity;
  double _gyroSensitivity;
};
