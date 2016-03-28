CON
  _CLKMODE = xtal1 + pll16x
  _XINFREQ = 6_000_000

  LED_PIN = 0 ' Use pin A0
  SCLK_PIN = 1
  MISO_PIN = 2
  MOSI_PIN = 4
  CE_PIN = 5
VAR 
    LONG outdata
    BYTE data
    BYTE inbyte
    LONG indata
OBJ 
    spi: "spi-dispatch" 
PUB main

    spi.Start(SCLK_PIN, MOSI_PIN, MISO_PIN, CE_PIN)
    repeat              
        waitcnt(cnt + CLKFREQ / 10)
        spi.TestSend

