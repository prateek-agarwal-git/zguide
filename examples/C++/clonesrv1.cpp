#include "zhelpers.hpp"
#include <iostream>
#include <unordered_map>
#include <zmq.hpp>

using namespace std;

int main() {
  // Prepare our context and publisher socket
  s_catch_signals();
  zmq::context_t ctx(1);
  zmq::socket_t publisher(ctx, ZMQ_PUB);
  publisher.bind("tcp://*:5555");
  s_sleep(
      5000); // Sleep for a short while to allow connections to be established

  // Initialize key-value map and sequence
  unordered_map<string, string> kvmap;
  int64_t sequence = 0;
  srand(time(NULL));

  while (!s_interrupted) {
    // Distribute as key-value message
    zmq::message_t kvmsg; // Allocate enough space for key-value message
    int key = rand() % 10000;
    int body = rand() % 1000000;
    std::string msg = std::to_string(key) + "=" + std::to_string(body);
    kvmsg.rebuild(msg.size());
    std::memcpy(kvmsg.data(), msg.c_str(), msg.size());
    publisher.send(kvmsg, zmq::send_flags::none); // Send key-value message

    // Store key-value pair in map
    kvmap[to_string(key)] = to_string(body);
    sequence++;

    // Sleep for a short while before sending the next message
    s_sleep(1000);
  }

  cout << "Interrupted" << endl;
  cout << sequence << " messages out" << endl;
  return 0;
}