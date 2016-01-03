'' =================================================================================================
''
''   File....... jm_servo8_adv.spin
''   Purpose.... Advanced servo control without PASM!
''   Author..... Jon "JonnyMac" McPhalen 
''               Copyright (c) 2011 Jon McPhalen
''               -- see below for terms of use
''   E-mail..... jon@jonmcphalen.com
''   Started.... 
''   Updated.... 03 AUG 2011
''
'' =================================================================================================

'' Theory of Operation
'' ───────────────────
'' The servo driver is called with the number of servos to operate (1 to 8), the first pin of the
'' servo group (0 to 28-servos), and the default timing values for each.  Once the driver is
'' started it will turn each servo on for its specified timing (us) before advancing to the next;
'' this means that only one servo is receiving a command pulse at any given time and minimizes the
'' load on the system.  After the final servo pulse the driver waits for the expiration of the
'' 20ms servo "frame" before starting again.
''
'' Each servo pulse is put into its own 2500us "slot" so that the leading-edge-to-leading-edge
'' timing for every servo is exactly 20ms.  The ctra module is used for pulse generation which allows
'' for sub-microsecond pulse accuracy.  While the pulse is generated, the position value is updated
'' if required to move it toward its intended target; this provides speed control in the driver.
''
''          │─────────────────────│      servo "frame" (20ms) = 8 "slots" (2500us each)
''
'' Servo 0   pulse timing 0.6ms to 2.4ms
'' Servo 1 
'' Servo 2 
'' Servo 3 
'' Servo 4 
'' Servo 5 
'' Servo 6 
'' Servo 7 
''
'' Legal servo values are 0.6ms to 2.4ms but user must check with servo vendor on actual limits for
'' servos in use.


con

  SVO_MIN  = 0_600                                              ' for 180 degree servos
  SVO_MAX  = 2_400

  CH_COUNT = 8
  CH_LAST  = CH_COUNT-1
  

var

  long  us001                                                   ' ticks per us

  long  smin                                                    ' min position
  long  smax                                                    ' max position

  long  pos[CH_COUNT]                                           ' current position (usecs)
  long  target[CH_COUNT]                                        ' target position (usecs)
  long  delta[CH_COUNT]                                         ' delta/cycle (usecs)

  long  cog                                                     ' cog # of servo driver
  long  stack[16]                                               ' stack for Spin cog
 
  
pub start(count, base)

'' Start servo cog
'' -- count is number of servo outputs (up to eight)
'' -- base is first pin of contiguous group

  return startx(count, base, SVO_MIN, SVO_MAX, 1_500)           ' start with standard values


pub startx(count, base, posmin, posmax, startup) | ok

'' Start servo cog
'' -- count is number of servo outputs (up to eight)
'' -- base is first pin of contiguous group
'' -- posmin and posmax expressed in microseconds
'' -- startup is starting value (us) for all outputs

  us001   := clkfreq / 1_000_000                                ' ticks per microsecond

  count   := 1 #> count <# CH_COUNT                             ' keep legal

  smin    := posmin #> SVO_MIN                                  ' set range
  smax    := posmax <# SVO_MAX

  stop                                                          ' stop if already running
  set_all(startup, 0)                                           ' set start-up values                                             
  
  ok := cog := cognew(servo8(count, base), @stack) + 1          ' launch servo cog

  return ok


pub startx2(count, base, posmin, posmax, pntr) | ok, ch

'' Start servo cog
'' -- count is number of servo outputs (up to eight)
'' -- base is first pin of contiguous group
'' -- posmin and posmax expressed in microseconds
'' -- pntr is address of table/array (of longs) with position start values (us)

  us001   := clkfreq / 1_000_000                                ' ticks per microsecond

  count   := 1 #> count <# CH_COUNT                             ' keep legal
  
  smin    := posmin #> SVO_MIN                                  ' set range
  smax    := posmax <# SVO_MAX

  stop                                                          ' stop if already running
  repeat ch from 0 to (count-1)                                 ' set starting positions
    set(ch, long[pntr][ch], 0)                                  '  for each channel            
  
  ok := cog := cognew(servo8(count, base), @stack) + 1          ' launch servo cog

  return ok
  

pub stop

'' Stop servo cog if running

  if cog
    cogstop(cog~ - 1)                                           ' stop the cog


pub set(ch, newpos, speed)

'' Set selected servo (ch) to new position
'' -- newpos specified in microseconds
'' -- speed specified in us change per second (0 = no speed, direct move)

  if ((ch => 0) and (ch =< CH_LAST))                            ' legal channel?
    newpos := smin #> newpos <# smax                            ' keep legal
    target[ch] := newpos                                        ' set new target
    if (speed =< 0)                                             ' check speed
      delta[ch] := 0                                            ' if 0
      pos[ch] := newpos                                         '  move now
    else
      ' round up
      delta[ch] := ((speed * 10 / 50) + 5) / 10 #> 1            ' divide by 50/updates per second 


pub set_all(newpos, speed)

'' Set all servos to new position
'' -- newpos specified in microseconds
'' -- speed specified in us change per second (0 = no speed, direct move)

  newpos := smin #> newpos <# smax                              ' keep legal
  speed  := 0 #> speed

  longfill(@target, newpos, 8)                                  ' update all positions 
  if (speed == 0)                                               ' if 0
    longfill(@pos, newpos, 8)                                   '   move now
  else
    ' round up
    speed := ((speed * 10 / 50) + 5) / 10 #> 1                  ' divide by 50/updates per second
  longfill(@delta, speed, 8)                                    ' update speed settings   


pub move(ch, newpos, ms) | frames, mdelta

'' Moves select servo to newpos in ~ms milliseconds
'' -- ms granularity is 20 (servo frame timing)
'' -- not accurate at low ms values for long moves

  if ((ch => 0) and (ch =< CH_LAST))                            ' legal channel?
    newpos := smin #> newpos <# smax                            ' keep legal
    frames := ms / 20                                           ' calc frames in move
    mdelta := ||(newpos - pos[ch]) * 10 / frames                ' calc 0.1us/frame
    mdelta := ((mdelta + 5) / 10) #> 1                          ' round up to 1us/frame
    if (frames < 2)
      pos[ch] := newpos                                         '   move now
      target[ch] := newpos
    else
      target[ch] := newpos                                      '   set new target            
      delta[ch] := mdelta                                       '   and movement delta/frame  


pub move_all(newpos, ms) | ch

'' Moves all servos to newpos in ~ms milliseconds
'' -- ms resolution is 20 (servo frame timing)
'' -- not accurate at low ms values for long moves

  repeat ch from 0 to 7
    move(ch, newpos, ms)


pub position(ch)

'' Returns position (current output command to servo) of channel

  if ((ch => 0) and (ch =< CH_LAST))                            ' legal channel?
    return pos[ch]                                              '  return position 
  else
    return -1                                                   ' bad channel specified

  
pub at_target(ch)

'' Returns True when channel is at desired target

  if ((ch => 0) and (ch =< CH_LAST))                            ' legal channel?
    return (pos[ch] == target[ch])                              '  return movement status
  else
    return false


pub wait(ch)

'' Waits (blocks program) until servo channel reaches target position

  if ((ch => 0) and (ch =< CH_LAST))                            ' legal channel?
    repeat while (pos[ch] <> target[ch])

 
pri servo8(count, base) | slot, idx                             ' launch with cognew

'' Runs up to 8 servos
'' -- count is number of servos (1 to 8)
'' -- base is LSB pin of contiguous group
'' -- runs in own cog; uses ctra

  count := 1 #> count <# 8                                      ' keep legal

  outa := 0                                                     ' all off
  dira := ($FF >> (8 - count)) << base                          ' set outputs

  frqa := 1                                                     ' preset for counter
  phsa := 0 

  slot := cnt                                                   ' start slot timing
  repeat
    repeat idx from 0 to 7                                      ' run 8 slots (20ms)
      if (idx < count)                                          ' active channel?
        ctra := (%00100 << 26 ) | (base + idx)                  ' PWM/NCO mode on servo pin
        phsa := -(pos[idx] * us001)                             ' set pulse timing
        if (pos[idx] < target[idx])                             ' update for speed
          pos[idx] := (pos[idx] + delta[idx]) <# target[idx]
        elseif (pos[idx] > target[idx])
          pos[idx] := (pos[idx] - delta[idx]) #> target[idx]
      waitcnt(slot += (2_500 * us001))                          ' let slot finish
      ctra := 0                                                 ' release ctra from pin 


dat

{{

  Terms of Use: MIT License

  Permission is hereby granted, free of charge, to any person obtaining a copy of this
  software and associated documentation files (the "Software"), to deal in the Software
  without restriction, including without limitation the rights to use, copy, modify,
  merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
  permit persons to whom the Software is furnished to do so, subject to the following
  conditions:

  The above copyright notice and this permission notice shall be included in all copies
  or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
  INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
  PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
  CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
  OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

}}