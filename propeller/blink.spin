CON
  _CLKMODE = xtal1 + pll16x
  _XINFREQ = 6_000_000

  MY_LED_PIN = 0 ' Use pin A0

PUB main
  DIRA[MY_LED_PIN] := 1 ' Set the LED pin to an output

  repeat ' Repeat forever
    OUTA[MY_LED_PIN] := 1  ' Turn the LED on
    waitcnt(cnt + clkfreq) ' Wait 1 second
                           ' cnt is the clock tick counter,
                           ' clkfreq is the number of clock ticks in a second
    OUTA[MY_LED_PIN] := 0  ' Turn the LED off
    waitcnt(cnt + clkfreq) ' Wait 1 second