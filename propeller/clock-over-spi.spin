CON
  _CLKMODE = xtal1 + pll16x
  _XINFREQ = 6_000_000

  LED_PIN = 0 ' Use pin A0
  SCLK_PIN = 1
  MISO_PIN = 2
  CE_PIN = 5
VAR 
    LONG clock
    BYTE data
OBJ 
PUB main
    DIRA[LED_PIN] := 1 ' Set the LED pin to an output
    DIRA[SCLK_PIN] := 0
    DIRA[CE_PIN] := 0
    DIRA[MISO_PIN] := 1
    repeat
       ' wait for chip enaple
       repeat until not ina[CE_PIN]
       outa[LED_PIN] := 1
       clock := CNT

       repeat 4
       	      data := clock >< 8 
              clock >>= 8
              OUTA[MISO_PIN] := data & 1
              repeat 8
        	      repeat until ina[SCLK_PIN]      	  	     
                      data >>= 1
                      repeat until not ina[SCLK_PIN]
                      OUTA[MISO_PIN] := data & 1

       repeat until ina[CE_PIN]
       outa[LED_PIN] := 0

