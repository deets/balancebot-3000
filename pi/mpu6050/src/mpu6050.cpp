#include "mpu6050.hpp"

#include <iostream>

const int WHO_AM_I = 0x75;

const int GYRO_CONFIG = 0x1B;
const int PWR_MGMT_1 = 0x6B;
const int INT_PIN_CFG = 0x37;
const int INT_ENABLE = 0x38;
const int INT_STATUS = 0x3A;

#define INT_FSYNC_EN (1 << 2)
#define INT_RD_CLEAR  (1 << 4)

const int ACCEL_XOUT_H = 0x3B;
const int ACCEL_YOUT_H = 0x3D;
const int ACCEL_ZOUT_H = 0x3F;
const int GYRO_XOUT_H = 0x43;
const int GYRO_YOUT_H = 0x45;
const int GYRO_ZOUT_H = 0x47;

const int SMPRT_DIV = 0x25;
const int USER_CTRL = 0x6A;

const int FIFO_EN = 0x23;
const int FIFO_COUNT_H = 0x72;
const int FIFO_COUNT_L = 0x73;
const int FIFO_RW = 0x74;

#define CLKSEL_INT_8MHZ (0)
#define CLKSEL_PLL_XGYRO (1)
#define CLKSEL_PLL_YGYRO (2)
#define CLKSEL_PLL_ZGYRO (3)
#define CLKSEL_PLL_EXT32K (4)
#define CLKSEL_PLL_EXT19M (5)
#define CLKSEL_STOP (7)

#define DEVICE_RESET 1 << 7

#define XG_FIFO_EN (1 << 6)
#define YG_FIFO_EN (1 << 5)
#define ZG_FIFO_EN (1 << 4)
#define ACCEL_FIFO_EN (1 << 3)

#define INT_FIFO_EN (1 << 4)
#define FIFO_OFLOW_INT (1 << 4)
#define USR_FIFO_EN (1 << 6)
#define USR_FIFO_RST (1 << 2)


MPU6050::MPU6050(I2CBus& bus, GYRO_SENSITIVITY gs, I2C_ADDRESS address)
  : _accSensitivity(16384.0)
{
  bus.device(static_cast<uint8_t>(address));
  bus.write_byte(PWR_MGMT_1, DEVICE_RESET);
  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  bus.write_byte(PWR_MGMT_1, CLKSEL_PLL_XGYRO);
  // bus.write_byte(CONFIG, 0);
  // bus.write_byte(GYRO_CONFIG, 0);
  // bus.write_byte(ACCEL_CONFIG, 0);
  // bus.write_byte(FF_THR, 0);
  // bus.write_byte(FF_DUR, 0);
  // bus.write_byte(MOT_THR, 0);
  // bus.write_byte(MOT_DUR, 0);
  // bus.write_byte(ZRMOT_THR, 0);
  // bus.write_byte(ZRMOT_DUR, 0);
  // bus.write_byte(FIFO_EN, 0);
  bus.write_byte(
      GYRO_CONFIG,
      ((uint8_t)gs) << 3
  );

  switch(gs) {
  case GYRO_SENSITIVITY::DEG250PS:
    _gyroSensitivity = 131;
    break;
  case GYRO_SENSITIVITY::DEG500PS:
    _gyroSensitivity = 65.5;
    break;
  case GYRO_SENSITIVITY::DEG1000PS:
    _gyroSensitivity = 32.8;
    break;
  case GYRO_SENSITIVITY::DEG2000PS:
    _gyroSensitivity = 16.4;
    break;
  }

}

MPU6050::~MPU6050() = default;

IMUData MPU6050::sample(I2CBus& bus, bool waitForData) {
  using namespace std::chrono;

  IMUData res;

  // wait until there is actually new data,
  // if we are asked to do so

  while(waitForData) {
    int status = bus.read_byte(INT_STATUS);
    if(0x01 & status)
      break;
    res.waitcnt++;
  }
  res.timestamp = steady_clock::now();

  double iaccX = static_cast<int16_t>(bus.read_word(ACCEL_XOUT_H));
  double iaccY = static_cast<int16_t>(bus.read_word(ACCEL_YOUT_H));
  double iaccZ = static_cast<int16_t>(bus.read_word(ACCEL_ZOUT_H));
  double igyroX = static_cast<int16_t>(bus.read_word(GYRO_XOUT_H));
  double igyroY = static_cast<int16_t>(bus.read_word(GYRO_YOUT_H));
  double igyroZ = static_cast<int16_t>(bus.read_word(GYRO_ZOUT_H));

  res.accX = iaccX / _accSensitivity;
  res.accY = iaccY / _accSensitivity;
  res.accZ = iaccZ / _accSensitivity;
  res.gyroX = igyroX / _gyroSensitivity;
  res.gyroY = igyroY / _gyroSensitivity;
  res.gyroZ = igyroZ / _gyroSensitivity;

  return res;
}
