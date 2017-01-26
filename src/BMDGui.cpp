
#include <iostream>

#include "BMDGui.h"
#include "ui_mainwindow.h"

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
  connect(on_button_, SIGNAL(pressed()), this, SLOT(handleOn()));
  connect(off_button_, SIGNAL(pressed()), this, SLOT(hendleOff()));
  connect(start_button_, SIGNAL(pressed()), this, SLOT(handleStart()));
  connect(stop_button_, SIGNAL(pressed()), this, SLOT(hendleStop()));
}

MainWindow::~MainWindow() {
  delete ui;
}

void MainWindow::closeEvent(QCloseEvent *bar) {
  std::cout << 5 << std::endl;;
  bar->accept();
}

void MainWindow::handleOn() {
  std::cout << 1 << std::endl;
}

void MainWindow::hendleOff() {
  std::cout << 4 << std::endl;
}

void MainWindow::handleStart() {
  std::cout << 2 << std::endl;
}

void MainWindow::hendleStop() {
  std::cout << 3 << std::endl;
}