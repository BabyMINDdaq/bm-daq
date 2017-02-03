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

#include <iostream>

#include "libufe-core.h"

#include "BMDGui.h"
#include "ui_mainwindow.h"

using namespace std;

MainWindow::MainWindow(QWidget *parent)
: QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  wdg_ = new QWidget(this);
  wdg_->setAttribute( Qt::WA_DeleteOnClose );
  setCentralWidget(wdg_);

  // Create the button, make "this" the parent
  on_button_ = new QPushButton("On",this);
  on_button_->move(15, 10);

  off_button_ = new QPushButton("Off",this);
  off_button_->move(15, 50);

  start_button_ = new QPushButton("Start",this);
  start_button_->move(15, 90);

  stop_button_ = new QPushButton("Stop",this);
  stop_button_->move(15, 130);

  // Connect button signal to appropriate slot
  connect(on_button_,    SIGNAL(pressed()), this, SLOT(handleOn()));
  connect(off_button_,   SIGNAL(pressed()), this, SLOT(hendleOff()));
  connect(start_button_, SIGNAL(pressed()), this, SLOT(handleStart()));
  connect(stop_button_,  SIGNAL(pressed()), this, SLOT(hendleStop()));

  zmq_ctx_ = zmq_ctx_new ();
  publisher_socket_ = zmq_socket (zmq_ctx_, ZMQ_PUB);
  /*int rc = */zmq_bind(publisher_socket_, "tcp://*:6120");
}

MainWindow::~MainWindow() {
  delete ui;
}

void MainWindow::sendCommand(int command) {
  int rc = zmq_send( this->publisher_socket_,
                       &command,
                       sizeof(command),
                       0);

  if (rc != sizeof(command)) {
    cerr << "!!!Error: ZMQ send error." << endl;
    this->close();
  }
}

void MainWindow::closeEvent(QCloseEvent *bar) {
  int x(5);
  cout << x << endl;
  this->sendCommand(x);
}

void MainWindow::handleOn() {
  int x(1);
  cout << x << endl;
  this->sendCommand(x);
}

void MainWindow::hendleOff() {
  int x(4);
  cout << x << endl;
  this->sendCommand(x);
}

void MainWindow::handleStart() {
  int x(2);
  cout << x << endl;
  this->sendCommand(x);
}

void MainWindow::hendleStop() {
  int x(3);
  cout << x << endl;
  this->sendCommand(x);
}