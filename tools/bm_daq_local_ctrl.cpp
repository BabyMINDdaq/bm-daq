#include <iostream>

#include "GetBackTrace.h"
#include "UFEError.h"
#include "BMDStateMachine.h"

using namespace std;

int main(int argc, char** argv) {

  SetErrorHdlr();

//   try {

  Fsm fsm;
  fsm.setUserFactory<UFEStateFactory>();

  auto localInput = [] {
    this_thread::sleep_for(std::chrono::milliseconds(10));
    int choice;
    cout << "Enter 1/2/3/4/>4 (On / Start / Stop / Off / Exit): \n";
    cin  >> choice;

    return choice;
  };

  fsm.start(localInput, 0);

//   } catch (UFEError &e) {
//     cerr << e.getDescription() << endl;
//     cerr << e.getLocation() << endl;
//     return 1;
//   }

  return 0;
}

