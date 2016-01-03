
for spin in *.spin; 
do
    openspin -Llib $spin
done

sudo ~/software/vc/propeller-hat/software/p1load/p1load test.binary
