/** This file is part of BabyMINDdaq software package. This software
 * package is designed for internal use for the Baby MIND detector
 * collaboration and is tailored for this use primarily.
 *
 * BabyMINDdaq is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * BabyMINDdaq is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with BabyMINDdaq.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  \author   Yordan Karadzhov <Yordan.Karadzhov \at cern.ch>
 *            University of Geneva
 *
 *  \created  Jan 2017
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>
#include <QPushButton>
// #include <QUdpSocket>

#include <zmq.hpp>

#include "libufe.h"

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
  void sendCommand(int command);

  Ui::MainWindow *ui;
  void *zmq_ctx_;
  void *publisher_socket_;
};

#endif // MAINWINDOW_H
