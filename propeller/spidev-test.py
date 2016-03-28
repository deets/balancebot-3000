import os
import struct
import spidev
import time
import serial
import threading
from functools import partial


BUS, DEVICE = 0, 0

def reader(conn):
    while True:
        print(conn.read().decode('ascii'), end='')


def main():
    s = serial.Serial('/dev/ttyAMA0',115200)
    t = threading.Thread(target=partial(reader, s))
    t.daemon = True
    #t.start()

    spi = spidev.SpiDev()
    spi.open(BUS, DEVICE)
    # max speed for spin-based SPI
    spi.max_speed_hz = 18000
    spi.mode = 1

    to_send = [1, 2, 3, 4]

    print("sampletime;timestamp")
    while True:
        before = time.time()
        result = bytearray(spi.xfer(to_send))
        mpoint = (time.time() + before) / 2.0
        ts = struct.unpack("<L", result)[0]
        print("{};{}".format(mpoint, ts))


if __name__ == "__main__":
    main()
