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

    /*
    login loginwindow(db);
    switch (loginwindow.exec()) {
    case QDialog::Rejected:
        QTimer::singleShot(0, this, SLOT(close()));
        break;
    } */
    email = "lilpush@ya.ru"; // loginwindow.returnEmail();

    on_pushButton_clicked();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    QSqlQuery qry(db);
    qry.prepare("SELECT COUNT(*) FROM bill "
                "INNER JOIN history ON history.idBill = bill.idBill "
                "INNER JOIN user ON user.idUser = bill.idUser "
                "WHERE history.date_finish IS NULL AND user.user_email = :email ");
    qry.bindValue(":email", email);
    qry.exec();
    qry.first();

    if (qry.value(0).toInt() == 0) {
       ui->stackedWidget->setCurrentIndex(0);
       reloadComboBox();
    } else {
        ui->stackedWidget->setCurrentIndex(2);
    }
}


void MainWindow::on_pushButton_2_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);

    reloadInfo();
}


void MainWindow::on_comboBox_currentIndexChanged(const QString &arg1)
{
    QStringList temp = arg1.split(" | ");

    if (temp.length() != 2) {
        return;
    }
    ui->tableWidget_2->setColumnCount(1);
    QSqlQuery qry(db);
    qry.prepare("SELECT color_name, vehicle.fuel, trademark.trademark_cost, (SELECT COUNT(*) FROM repaired WHERE repaired.idVehicle = vehicle.idVehicle) FROM vehicle  "
                "INNER JOIN color ON color.idColor = vehicle.idColor "
                "INNER JOIN trademark ON trademark.idTrademark = vehicle.idTrademark "
                "WHERE (idVehicle = (SELECT idVehicle FROM PMV WHERE PMV.pmv_number = :number) OR "
                " idVehicle = (SELECT idVehicle FROM machine WHERE machine.state_number = :number)) "
                );
    qry.bindValue(":number", temp[1]);
    qry.exec();
    qry.first();
    ui->tableWidget_2->setItem(0, 0, new QTableWidgetItem(temp[1]));
    ui->tableWidget_2->setItem(1, 0, new QTableWidgetItem(temp[0]));
    ui->tableWidget_2->setItem(2, 0, new QTableWidgetItem(qry.value(0).toString()));
    ui->tableWidget_2->setItem(3, 0, new QTableWidgetItem(qry.value(1).toString() + " %"));
    ui->tableWidget_2->setItem(4, 0, new QTableWidgetItem(qry.value(2).toString()));
    ui->tableWidget_2->setItem(5, 0, new QTableWidgetItem(qry.value(3).toString()));
}

void MainWindow::reloadComboBox()
{

    QSqlQuery qry(db);
    ui->comboBox->clear();
    qry.prepare("SELECT trademark_name, pmv_number  "
                " FROM vehicle "
                "INNER JOIN trademark ON trademark.idTrademark = vehicle.idTrademark "
                "INNER JOIN PMV ON PMV.idVehicle = vehicle.idVehicle "
                "WHERE vehicle.idVehicle in (SELECT idVehicle FROM history INNER JOIN bill ON history.idBill = bill.idBill where history.date_finish NOT NULL ) "
               );
    qry.exec();
    while (qry.next()) {
        ui->comboBox->addItem(qry.value(0).toString() + " | " + qry.value(1).toString());
    }
    qry.prepare("SELECT trademark_name, state_number  "
                 " FROM vehicle "
                 "INNER JOIN trademark ON trademark.idTrademark = vehicle.idTrademark "
                 "INNER JOIN machine ON machine.idVehicle = vehicle.idVehicle "
                 "WHERE vehicle.idVehicle not in (SELECT bill.idVehicle FROM history INNER JOIN bill ON history.idBill = bill.idBill where history.date_finish IS NULL ) "
                 );
     qry.exec();
     while (qry.next()) {
         ui->comboBox->addItem(qry.value(0).toString() + " | " + qry.value(1).toString());
     }

}

void MainWindow::reloadInfo()
{
    ui->tableWidget->setRowCount(0);

    QSqlQuery qry(db);

    if (email == "admin") {
        qry.prepare("SELECT COUNT(*) "
                    " FROM history INNER JOIN bill ON history.idBill = bill.idBill "
                    "INNER JOIN discount ON bill.idDiscount = discount.idDiscount "
                    "INNER JOIN vehicle ON vehicle.idVehicle = bill.idVehicle "
                    "INNER JOIN color ON color.idColor = vehicle.idColor "
                    "INNER JOIN tenant ON tenant.idTenant = vehicle.idTenant "
                    "INNER JOIN trademark ON trademark.idTrademark = vehicle.idTrademark "
                    "INNER JOIN user ON bill.idUser = user.idUser "
                    "WHERE color_name LIKE :color AND trademark_name LIKE :trademark");
        qry.bindValue(":color", QString("%%1%").arg(ui->lineEdit->text()));
        qry.bindValue(":trademark", QString("%%1%").arg(ui->lineEdit_2->text()));
        qry.exec();
        qry.first();
        ui->tableWidget->setRowCount(qry.value(0).toInt());

        qry.prepare("SELECT history.idHistory, history.date_start, history.date_finish, bill.bill_status, bill.bill_total, "
                    "bill.billing_time, discount.discount_size, trademark.trademark_name, tenant.tenant_name, color.color_name "
                    " FROM history INNER JOIN bill ON history.idBill = bill.idBill "
                    "INNER JOIN discount ON bill.idDiscount = discount.idDiscount "
                    "INNER JOIN vehicle ON vehicle.idVehicle = bill.idVehicle "
                    "INNER JOIN color ON color.idColor = vehicle.idColor "
                    "INNER JOIN tenant ON tenant.idTenant = vehicle.idTenant "
                    "INNER JOIN trademark ON trademark.idTrademark = vehicle.idTrademark "
                    "INNER JOIN user ON bill.idUser = user.idUser "
                    "WHERE color_name LIKE :color AND trademark_name LIKE :trademark");
        qry.bindValue(":color", QString("%%1%").arg(ui->lineEdit->text()));
        qry.bindValue(":trademark", QString("%%1%").arg(ui->lineEdit_2->text()));
        qry.exec();
    } else {
        qry.prepare("SELECT COUNT(*) "
                    " FROM history INNER JOIN bill ON history.idBill = bill.idBill "
                    "INNER JOIN discount ON bill.idDiscount = discount.idDiscount "
                    "INNER JOIN vehicle ON vehicle.idVehicle = bill.idVehicle "
                    "INNER JOIN color ON color.idColor = vehicle.idColor "
                    "INNER JOIN tenant ON tenant.idTenant = vehicle.idTenant "
                    "INNER JOIN trademark ON trademark.idTrademark = vehicle.idTrademark "
                    "INNER JOIN user ON bill.idUser = user.idUser "
                    "WHERE user.user_email = :email AND color_name LIKE :color AND trademark_name LIKE :trademark");
        qry.bindValue(":email", email);
        qry.bindValue(":color", QString("%%1%").arg(ui->lineEdit->text()));
        qry.bindValue(":trademark", QString("%%1%").arg(ui->lineEdit_2->text()));
        qry.exec();
        qry.first();
        ui->tableWidget->setRowCount(qry.value(0).toInt());

        qry.prepare("SELECT history.idHistory, history.date_start, history.date_finish, bill.bill_status, bill.bill_total, "
                    "bill.billing_time, discount.discount_size, trademark.trademark_name, tenant.tenant_name, color.color_name "
                    " FROM history INNER JOIN bill ON history.idBill = bill.idBill "
                    "INNER JOIN discount ON bill.idDiscount = discount.idDiscount "
                    "INNER JOIN vehicle ON vehicle.idVehicle = bill.idVehicle "
                    "INNER JOIN color ON color.idColor = vehicle.idColor "
                    "INNER JOIN tenant ON tenant.idTenant = vehicle.idTenant "
                    "INNER JOIN trademark ON trademark.idTrademark = vehicle.idTrademark "
                    "INNER JOIN user ON bill.idUser = user.idUser "
                    "WHERE user.user_email = :email AND color_name LIKE :color AND trademark_name LIKE :trademark");
        qry.bindValue(":email", email);
        qry.bindValue(":color", QString("%%1%").arg(ui->lineEdit->text()));
        qry.bindValue(":trademark", QString("%%1%").arg(ui->lineEdit_2->text()));
        qry.exec();
    }


    int i = 0;
    while (qry.next()) {
        qDebug() << QString::number(qry.value(0).toInt());
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


void MainWindow::on_lineEdit_textChanged(const QString &arg1)
{
    reloadInfo();
}


void MainWindow::on_lineEdit_2_textChanged(const QString &arg1)
{
    reloadInfo();
}


void MainWindow::on_lineEdit_3_textChanged(const QString &arg1)
{
    reloadInfo();
}


void MainWindow::on_pushButton_3_clicked()
{
    QSqlQuery qry(db);
    qry.prepare("SELECT idVehicle FROM PMV WHERE pmv_number = :number");
    qry.bindValue(":number", ui->comboBox->currentText().split(" | ")[1]);
    qry.exec();
    qry.first();
    int idTemp = qry.value(0).toInt();
    if (idTemp != 0) {
        qry.prepare("INSERT INTO bill (bill_status, idUser, idDiscount, idVehicle) VALUES (1, (SELECT idUser FROM user WHERE user_email = :email), 3, :id)");
        qry.bindValue(":email", email);
        qry.bindValue(":id", idTemp);
        qDebug() << "asd";
    } else {
        qry.prepare("INSERT INTO bill (bill_status, idUser, idDiscount, idVehicle) VALUES (0, (SELECT idUser FROM user WHERE user_email = :email), 3,"
                    " (SELECT idVehicle FROM machine WHERE state_number = :id))");
        qry.bindValue(":email", email);
        qry.bindValue(":id", ui->comboBox->currentText().split(" | ")[1]);
    }
    qDebug() << qry.exec();
    db.commit();
    on_pushButton_clicked();
}


void MainWindow::on_pushButton_4_clicked()
{
    QSqlQuery qry(db);
    qry.prepare("UPDATE history SET date_finish = datetime() WHERE idBill = "
                "(SELECT idBill FROM bill WHERE idUser = (SELECT idUser FROM user WHERE user_email = :email) AND billing_time is NULL)");
    qry.bindValue(":email", email);
    //qry.exec();
    qDebug() << qry.exec();
    qDebug() << qry.lastError().text();
    /*qry.prepare("UPDATE bill SET billing_time = date() WHERE idUser = (SELECT idUser FROM user WHERE user_email = :email) AND billing_time is NULL");
    qry.bindValue(":email", email);
    qry.exec();*/

    db.commit();
    on_pushButton_clicked();
}

