CON
  _CLKMODE = xtal1 + pll16x
  _XINFREQ = 6_000_000

  DIR_PIN = 13
  STEP_PIN = 14

VAR 

OBJ 
PUB main
    DIRA[DIR_PIN] := 1 
    DIRA[STEP_PIN] := 1 

    repeat
        waitcnt(cnt + clkfreq / 400)
        OUTA[STEP_PIN] ^= 1


