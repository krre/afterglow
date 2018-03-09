#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow() :
        ui(new Ui::MainWindow) {
    ui->setupUi(this);
}

MainWindow::~MainWindow() {
    delete ui;
}
