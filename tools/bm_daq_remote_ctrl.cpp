#include <iostream>

#include <zmq.hpp>

#include "libufe-tools.h"

#include "GetBackTrace.h"
#include "UFEError.h"
#include "BMDStateMachine.h"

using namespace std;

int main(int argc, char** argv) {

  SetErrorHdlr();

  int ip_arg   = get_arg_val('i', "ip-address", argc, argv);
  int port_arg = get_arg_val('p', "port",       argc, argv);

  if (ip_arg == 0 || port_arg ==0) {
    fprintf(stderr, "\nUsage: %s [OPTIONS] \n\n", argv[0]);
    fprintf(stderr, "    -i / --ip-address  <string> :  Message publisher IP or Hostname [ required ]\n\n");
    fprintf(stderr, "    -p / --port        <int>    :  Messages from localhost          [ required ]\n\n");
    return 1;
  }

  void *context = zmq_ctx_new ();
  void *subscriber = zmq_socket (context, ZMQ_SUB);

  string end_point("tcp://");
  end_point += argv[ip_arg];
  end_point += ":";
  end_point += argv[port_arg];

  int rc = zmq_connect (subscriber, end_point.c_str());
  if (rc != 0)
    return 1;

  rc = zmq_setsockopt (subscriber, ZMQ_SUBSCRIBE, NULL, 0);
  if (rc != 0)
    return 1;

  std::cout << "connected to " << end_point << " ..." << std::endl;

  auto remoteInput = [&] {
    this_thread::sleep_for(std::chrono::milliseconds(10));
    int choice;
    int rc = zmq_recv(subscriber, &choice, sizeof(choice), 0);
    if (rc != sizeof(choice)) {
      // Network error! Exit.
      return 5;
    }

    cout << "remoteInput: " << choice << endl;
    return choice;
  };

//   try {

  Fsm fsm;
  fsm.setUserFactory<UFEStateFactory>();
  fsm.start(remoteInput, 0);

//   } catch (UFEError &e) {
//     cerr << e.getDescription() << endl;
//     cerr << e.getLocation() << endl;
//     return 1;
//   }

  return 0;
}

