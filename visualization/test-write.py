import sys

import pprint
import nanomsg
import json
import time
from itertools import count
import math

uri = "tcp://*:5555"

socket = nanomsg.Socket(nanomsg.PAIR)
socket.bind(uri)

key = sys.argv[1] if len(sys.argv) > 1 else None

then = time.time()


for count in count():
    timestamp = time.time() - then
    value = math.sin(timestamp * .4) * 10
    data = dict(timestamp=timestamp, value=value)
    print data
    socket.send(json.dumps(data))
    time.sleep(.03)
