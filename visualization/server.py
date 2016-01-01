import os
import time
import tempfile
from functools import partial

import tornado.ioloop
import tornado.web
import tornado.websocket

from tornado.options import define, options, parse_command_line

import nanomsg

define("port", default=12345, help="run on the given port", type=int)
define("uri", default="tcp://192.168.2.102:5555", help="pull data from that pair socket", type=str)
define("split", default=False, help="split out incoming data into file")

class IndexHandler(tornado.web.RequestHandler):
    @tornado.web.asynchronous
    def get(self):
        self.render("index.html")


class WebSocketHandler(tornado.websocket.WebSocketHandler):

    CLIENTS = {}

    def open(self, *args):
        self.id = self.get_argument("Id")
        self.stream.set_nodelay(True)
        self.CLIENTS[self.id] = {
            "id": self.id,
            "object": self,
            "write_message": self.write_message,
        }


    def on_message(self, message):
        pass


    def on_close(self):
        if self.id in self.CLIENTS:
            del self.CLIENTS[self.id]


def imu_message_arrived(socket, split_write, *args):
    message = socket.recv()
    split_write(message)
    for wsh in WebSocketHandler.CLIENTS.values():
        wsh["write_message"](message)


def main():
    parse_command_line()

    if options.split:
        split_file = tempfile.mktemp(suffix="-mpu6050.log")
        print("Writing data to '%s'" % split_file)
        outf = open(split_file, 'wb')
        then = time.time()
        def split_write(message):
            outf.write(b"#" * 10)
            outf.write(b"\n")
            outf.write(message)
    else:
        def split_write(_message):
            pass

    settings = {
        "static_path": os.path.join(os.path.dirname(__file__), "static"),
    }

    app = tornado.web.Application(
        [
            (r'/', IndexHandler),
            (r'/ws', WebSocketHandler),
        ],
        **settings
    )

    socket = nanomsg.Socket(nanomsg.PAIR)
    print("Connecting to nanomsg on '%s'" % options.uri)
    socket.connect(options.uri)
    print("Server listening on 'http://localhost:%i/'" % options.port)
    app.listen(options.port)
    ioloop = tornado.ioloop.IOLoop.instance()



    ioloop.add_handler(
        socket.fd,
        partial(imu_message_arrived, socket, split_write),
        tornado.ioloop.IOLoop.READ | tornado.ioloop.IOLoop.WRITE | tornado.ioloop.IOLoop.ERROR
        )
    ioloop.start()


if __name__ == '__main__':
    main()
