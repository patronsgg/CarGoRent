#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "login.h"
#include <QTimer>
#include <QSqlQuery>
#include <QSqlError>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("./rent.db");
    db.open();

    login loginwindow(db);
    switch (loginwindow.exec()) {
    case QDialog::Rejected:
        QTimer::singleShot(0, this, SLOT(close()));
        break;
    }
    email = loginwindow.returnEmail();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);


}


void MainWindow::on_pushButton_2_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
    ui->tableWidget->setRowCount(0);

    QSqlQuery qry(db);

    qry.prepare("SELECT COUNT(*) "
                " FROM history INNER JOIN bill ON history.idBill = bill.idBill "
                "INNER JOIN discount ON bill.idDiscount = discount.idDiscount "
                "INNER JOIN vehicle ON vehicle.idVehicle = bill.idVehicle "
                "INNER JOIN color ON color.idColor = vehicle.idColor "
                "INNER JOIN tenant ON tenant.idTenant = vehicle.idTenant "
                "INNER JOIN trademark ON trademark.idTrademark = vehicle.idTrademark "
                "INNER JOIN user ON bill.idUser = user.idUser "
                "WHERE user.user_email = :email");
    qry.bindValue(":email", email);
    qry.exec();
    qry.first();
    ui->tableWidget->setRowCount(qry.value(0).toInt());

    qry.prepare("SELECT history.idHistory, history.date_start, history.date_finish, bill.bill_status, bill.bill_total,"
                "bill.billing_time, discount.discount_size, trademark.trademark_name, tenant.tenant_name, color.color_name, COUNT(*) "
                " FROM history INNER JOIN bill ON history.idBill = bill.idBill "
                "INNER JOIN discount ON bill.idDiscount = discount.idDiscount "
                "INNER JOIN vehicle ON vehicle.idVehicle = bill.idVehicle "
                "INNER JOIN color ON color.idColor = vehicle.idColor "
                "INNER JOIN tenant ON tenant.idTenant = vehicle.idTenant "
                "INNER JOIN trademark ON trademark.idTrademark = vehicle.idTrademark "
                "INNER JOIN user ON bill.idUser = user.idUser "
                "WHERE user.user_email = :email");
    qry.bindValue(":email", email);
    qry.exec();
    int i = 0;
    while (qry.next()) {
        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(QString::number(qry.value(0).toInt())));
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(qry.value(1).toString()));
        ui->tableWidget->setItem(i, 2, new QTableWidgetItem(qry.value(2).toString()));
        ui->tableWidget->setItem(i, 3, new QTableWidgetItem(QString::number(qry.value(3).toInt())));
        ui->tableWidget->setItem(i, 4, new QTableWidgetItem(QString::number(qry.value(4).toInt())));
        ui->tableWidget->setItem(i, 5, new QTableWidgetItem(qry.value(5).toString()));
        ui->tableWidget->setItem(i, 6, new QTableWidgetItem(QString::number(qry.value(6).toInt())));
        ui->tableWidget->setItem(i, 7, new QTableWidgetItem(qry.value(7).toString()));
        ui->tableWidget->setItem(i, 8, new QTableWidgetItem(qry.value(8).toString()));
        ui->tableWidget->setItem(i, 9, new QTableWidgetItem(qry.value(9).toString()));
        i++;
    }
}

