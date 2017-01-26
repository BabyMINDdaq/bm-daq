#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>
#include <QPushButton>
// #include <QUdpSocket>

#include <zmq.hpp>

#include "Fifo.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

private slots:
  void handleOn();
  void hendleOff();
  void handleStart();
  void hendleStop();

private:
  void closeEvent(QCloseEvent *bar);
  QWidget     *wdg_;
  QPushButton *on_button_;
  QPushButton *off_button_;
  QPushButton *start_button_;
  QPushButton *stop_button_;

private:
  Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
