#include <nanomsg/nn.h>
#include <nanomsg/pair.h>
#include <stdio.h>
#include <cassert>
#include <vector>
#include <algorithm>

int main() {
  auto uri = "ipc:///tmp/foobar";
  int socket = nn_socket (AF_SP, NN_PAIR);
  assert(socket >= 0);
  assert (nn_connect (socket, uri) >= 0);
  while(true) {
    char *buf = NULL;
    int result = nn_recv (socket, &buf, NN_MSG, 0);
    if (result > 0) {
      std::vector<char> message(result + 1);
      std::copy(buf, buf + result, std::begin(message));
      message[result] = '\0';
      printf("%s\n", message.data());
      nn_freemsg(buf);
    }
  }
}
