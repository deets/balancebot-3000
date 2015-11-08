import pprint
import nanomsg
import json
import sys
import time

uri = "tcp://192.168.2.102:5555"

socket = nanomsg.Socket(nanomsg.PAIR)
socket.connect(uri)

key = sys.argv[1] if len(sys.argv) > 1 else None

then = time.time()
while True:
    m = socket.recv()
    elapsed = time.time() - then
    then += elapsed

    try:
        data = json.loads(m)
        if key:
            data = data[key]
        pprint.pprint(data)
        print ("#" * 40) + " FPS: %.2f" % (1.0 / elapsed)

    except:
        print m
