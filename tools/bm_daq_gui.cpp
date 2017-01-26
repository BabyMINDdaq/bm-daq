#include "mainwindow.h"
#include <QApplication>


#include "GetBackTrace.h"
#include "UFEError.h"
#include "BMDStateMachine.h"

int startGui(Fifo<int> *in_fifo, int argc, char **argv) {
  QApplication a(argc, argv);
  MainWindow w;
  w.setCommandOutput(in_fifo);
  w.show();
  return a.exec();
}
int main(int argc, char **argv) {
  SetErrorHdlr();

  Fsm fsm;
  fsm.setUserFactory<UFEStateFactory>();
  Fifo<int> in_fifo(6);

  auto job = [&] {
    fsm.start1(&in_fifo);
  };
  std::thread t(job);

  startGui(&in_fifo, argc, argv);
  t.join();
  return 0;
}

