#include "i2c.hpp"

#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <sstream>
#include <stdexcept>


I2CBus::I2CBus(uint32_t busno) : _addr(-1)
{
  char filename[20];
  std::stringstream s;
  s << "/dev/i2c-" << busno;
  _file = open(s.str().c_str(), O_RDWR);
  if (_file < 0) {
    throw_error();
  }
}


void I2CBus::device(int addr) {
  if(addr != _addr) {
    if (ioctl(_file, I2C_SLAVE, addr) < 0) {
      throw_error();
    }
    _addr = addr;
  }
}


uint8_t I2CBus::read_byte(int reg) {
  auto res = i2c_smbus_read_byte_data(_file, reg);
  if (res < 0) {
      throw_error();
  } else {
    return (res & 0xff);
  }
  return 0;
}


uint16_t I2CBus::read_word(int reg) {
  int hi = i2c_smbus_read_byte_data(_file, reg);
  if (hi < 0) {
      throw_error();
  } else {
    int low = i2c_smbus_read_byte_data(_file, reg+1);
    if(low < 0) {
      throw_error();
    }
    return (uint16_t)(hi << 8 | low);
  }
}


void I2CBus::write_byte(int reg, uint8_t data) {
  auto res = i2c_smbus_write_byte_data(_file, reg, data);
  if (res < 0) {
    throw_error();
  }
}


void I2CBus::throw_error() {
  std::stringstream s;
  s << "I2CBus-error:" << std::strerror(errno);
  throw std::runtime_error(s.str());
}


I2CBus::~I2CBus() {
  close(_file);
}
