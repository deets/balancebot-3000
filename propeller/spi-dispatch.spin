'' =======
'' 
'' =======
VAR
   LONG foo
OBJ
    serial: "FullDuplexSerial"

PUB Start(clk_pin, mosi_pin, miso_pin, cs_pin) | i
  clk_mask := |< clk_pin
  mosi_mask := |< mosi_pin
  miso_mask := |< miso_pin
  cs_mask := |< cs_pin
  RESULT := cognew(@entrypoint, @foo) + 1
  serial.start( 31, 30, 0, 115200 )


PUB TestSend

  serial.Bin(cs_mask, 32)
  serial.Tx(45)
  serial.Bin(foo, 32)
  serial.Tx(10)

DAT
 ORG 0

entrypoint
                andn    dira, clk_mask
                andn    dira, mosi_mask
                andn    dira, cs_mask
                or      dira, miso_mask
                ' clear miso
                andn    outa, miso_mask
' wait while we are inactive
cs_inactive
                test    cs_mask, ina             wz
        if_nz   jmp     #cs_inactive
' setup data to transmit
                mov     output, cnt
' setup loop variables
                mov     bytecount, #4

byteloop
                mov     outbyte, output
' this is counterintuitive, but the 
' bits reversed are the ones *not* masked
                rev     outbyte, #24
                mov     bitmask, #1
                mov     bitcount, #8
bitloop
                test    outbyte, bitmask          wz
        if_nz   or      outa, miso_mask
        if_z    andn    outa, miso_mask

clock_low       test    clk_mask, ina            wz
        if_z    jmp     #clock_low       
' clock went hi, we should now acquire data

clock_hi        test    clk_mask, ina            wz
        if_nz   jmp     #clock_hi
' clock is low again, prepare next bit
                shl     bitmask, #1
                djnz    bitcount, #bitloop
' we transmitted a byte, get the next
                shr     output, #8
                djnz    bytecount, #byteloop 
' clear miso channel
                andn    outa, miso_mask
' wait until we become deactive again
cs_active
                test    cs_mask, ina             wz
        if_z    jmp     #cs_active
                jmp     #cs_inactive

clk_mask LONG 0
mosi_mask LONG 0
miso_mask LONG 0
cs_mask LONG 0
cs_and  LONG 0
output  LONG 0  
outbyte LONG 0
bytecount   LONG 0
bitcount LONG 0
bitmask LONG 0
FIT 496 