import os
import threading
from functools import partial

import tornado.ioloop
import tornado.web
import tornado.websocket

from tornado.options import define, options, parse_command_line

import nanomsg

define("port", default=12345, help="run on the given port", type=int)
define("uri", default="tcp://192.168.2.102:5555", help="pull data from that pair socket", type=str)

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


def imu_message_arrived(socket, *args):
    message = socket.recv()
    for wsh in WebSocketHandler.CLIENTS.values():
        wsh["write_message"](message)


def main():
    parse_command_line()

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
    socket.connect(options.uri)

    app.listen(options.port)
    ioloop = tornado.ioloop.IOLoop.instance()

    ioloop.add_handler(
        socket.fd,
        partial(imu_message_arrived, socket),
        tornado.ioloop.IOLoop.READ | tornado.ioloop.IOLoop.WRITE | tornado.ioloop.IOLoop.ERROR
        )
    ioloop.start()


if __name__ == '__main__':
    main()
