import pprint
import nanomsg
import json
import sys

uri = "tcp://192.168.2.102:5555"

socket = nanomsg.Socket(nanomsg.PAIR)
socket.connect(uri)

key = sys.argv[1] if len(sys.argv) > 1 else None

while True:
    m = socket.recv()
    try:
        data = json.loads(m)
        if key:
            data = data[key]
        pprint.pprint(data)
        print "#" * 40
    except:
        print m
