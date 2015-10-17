import tornado.ioloop
import tornado.web
import tornado.websocket
import threading
import json
from functools import partial

import nanomsg

from tornado.options import define, options, parse_command_line

define("port", default=8888, help="run on the given port", type=int)

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


app = tornado.web.Application([
    (r'/', IndexHandler),
    (r'/ws', WebSocketHandler),
])


def imu_message_arrived(socket, *args):
    message = socket.recv()
    for wsh in WebSocketHandler.CLIENTS.values():
        wsh["write_message"](message)


if __name__ == '__main__':

    uri = "tcp://192.168.2.102:5555"
    socket = nanomsg.Socket(nanomsg.PAIR)
    socket.connect(uri)

    parse_command_line()
    app.listen(options.port)
    ioloop = tornado.ioloop.IOLoop.instance()
    ioloop.add_handler(
        socket.fd,
        partial(imu_message_arrived, socket),
        tornado.ioloop.IOLoop.READ | tornado.ioloop.IOLoop.WRITE | tornado.ioloop.IOLoop.ERROR
        )
    ioloop.start()
