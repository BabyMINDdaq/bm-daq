/* This file is part of BabyMINDdaq software package. This software
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
 * along with BabyMINDdaq. If not, see <http://www.gnu.org/licenses/>.
 */


/**
 *  \file    BMDGui.h
 *  \brief   File containing declaration of the Baby MIND DAQ remote
 *  control GUI.
 *  \author  Yordan Karadzhov
 *  \date    Nov 2016
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// qt
#include <QMainWindow>
#include <QtWidgets>
#include <QPushButton>

// zeromq
#include <zmq.hpp>


namespace Ui {
class MainWindow;
}

/** \class MainWindow
 *  Main window of the remote control GUI.
 */
class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  /** Construct.  */
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

  void sendCommand(int command);

  Ui::MainWindow *ui;
  void *zmq_ctx_;
  void *publisher_socket_;
};

#endif // MAINWINDOW_H
