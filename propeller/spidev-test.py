import os
import struct
import spidev
import time

BUS, DEVICE = 0, 0

def main():
    spi = spidev.SpiDev()
    spi.open(BUS, DEVICE)
    # max speed for spin-based SPI
    spi.max_speed_hz = 18000
    spi.mode = 0

    to_send = [0] * 4

    while True:
        before = time.time()
        result = bytearray(spi.xfer(to_send))
        mpoint = (time.time() + before) / 2.0
        ts = struct.unpack("<L", result)[0]
        print("{};{}".format(mpoint, ts))


if __name__ == "__main__":
    main()
