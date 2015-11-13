#pragma once

#include <cstdint>

class I2CBus {

public:
  I2CBus(uint32_t busno);
  virtual ~I2CBus();
  void device(int addr);
  uint8_t read_byte(int reg);
  uint16_t read_word(int reg);
  void write_byte(int reg, uint8_t data);

private:
  void throw_error();
  int _file;
  int _addr;
};
