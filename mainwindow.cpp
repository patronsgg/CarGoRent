#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    if (db.open()) {
        qDebug() << "db open";
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

