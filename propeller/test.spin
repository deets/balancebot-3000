CON
  _CLKMODE = xtal1 + pll16x
  _XINFREQ = 6_000_000

  MY_LED_PIN = 0 ' Use pin A0
  SERVO_PIN = 3 ' Pin A3
OBJ 
    Servo: "servo"
PUB main
    Servo.start(1, SERVO_PIN)
    DIRA[MY_LED_PIN] := 1 ' Set the LED pin to an output
    'DIRA[SERVO_PIN] := 1
    repeat ' Repeat forever
        OUTA[MY_LED_PIN] := 1  ' Turn the LED on
        'OUTA[SERVO_PIN] := 1  ' Turn the LED on
        Servo.set(0, 800, 0)
    	waitcnt(cnt + clkfreq) ' Wait 1 second
        'servo.leftPulseLength(5)
        Servo.set(0, 2000, 0)
	OUTA[MY_LED_PIN] := 0  ' Turn the LED off
        'OUTA[SERVO_PIN] := 0  ' Turn the LED on
	waitcnt(cnt + clkfreq) ' Wait 1 second
	