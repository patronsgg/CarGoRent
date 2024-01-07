#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "login.h"
#include <QMessageBox>
#include <QTimer>
#include <QSqlQuery>
#include <QSqlError>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::reloadTable);
    timer->setInterval(2000);

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("./rent.db");
    db.open();
    this->setWindowTitle("CargoRent");

    /*
    login loginwindow(db);
    loginwindow.setWindowTitle("CargoRent");
    switch (loginwindow.exec()) {
    case QDialog::Rejected:
        QTimer::singleShot(0, this, SLOT(close()));
        break;
    }*/
    email = "lilpush@ya.ru";//loginwindow.returnEmail();
    typeAccount = 0;// loginwindow.returnType();

    loadPage();

    ui->pushButton_5->setText(email);

    on_pushButton_clicked();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    timer->stop();

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
        timer->start();
        reloadTable();
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
    if (typeAccount == 0) {
        qry.prepare("SELECT trademark_name, pmv_number  "
                    " FROM vehicle "
                    "INNER JOIN trademark ON trademark.idTrademark = vehicle.idTrademark "
                    "INNER JOIN PMV ON PMV.idVehicle = vehicle.idVehicle "
                    "WHERE vehicle.idVehicle in (SELECT idVehicle FROM history INNER JOIN bill ON history.idBill = bill.idBill where history.date_finish NOT NULL ) AND vehicle.fuel > 0 "
                   );
        qry.exec();
        while (qry.next()) {
            ui->comboBox->addItem(qry.value(0).toString() + " | " + qry.value(1).toString());
        }
        qry.prepare("SELECT trademark_name, state_number  "
                     " FROM vehicle "
                     "INNER JOIN trademark ON trademark.idTrademark = vehicle.idTrademark "
                     "INNER JOIN machine ON machine.idVehicle = vehicle.idVehicle "
                     "WHERE vehicle.idVehicle not in (SELECT bill.idVehicle FROM history INNER JOIN bill ON history.idBill = bill.idBill where history.date_finish IS NULL ) AND vehicle.fuel > 0"
                     );
         qry.exec();
         while (qry.next()) {
             ui->comboBox->addItem(qry.value(0).toString() + " | " + qry.value(1).toString());
         }
    } else {
        qry.prepare("SELECT trademark_name, pmv_number  "
                    " FROM vehicle "
                    "INNER JOIN trademark ON trademark.idTrademark = vehicle.idTrademark "
                    "INNER JOIN PMV ON PMV.idVehicle = vehicle.idVehicle "
                    "WHERE vehicle.idVehicle in (SELECT idVehicle FROM history INNER JOIN bill ON history.idBill = bill.idBill where history.date_finish NOT NULL ) AND vehicle.fuel < 100 "
                   );
        qry.exec();
        while (qry.next()) {
            ui->comboBox->addItem(qry.value(0).toString() + " | " + qry.value(1).toString());
        }
        qry.prepare("SELECT trademark_name, state_number  "
                     " FROM vehicle "
                     "INNER JOIN trademark ON trademark.idTrademark = vehicle.idTrademark "
                     "INNER JOIN machine ON machine.idVehicle = vehicle.idVehicle "
                     "WHERE vehicle.idVehicle not in (SELECT bill.idVehicle FROM history INNER JOIN bill ON history.idBill = bill.idBill where history.date_finish IS NULL ) AND vehicle.fuel < 100"
                     );
         qry.exec();
         while (qry.next()) {
             ui->comboBox->addItem(qry.value(0).toString() + " | " + qry.value(1).toString());
         }
    }

}

void MainWindow::reloadInfo()
{
    ui->tableWidget->setRowCount(0);

    QSqlQuery qry(db);
    if (typeAccount == 0) {
        if (email == "admin") {
            qry.prepare("SELECT COUNT(*) "
                        " FROM history INNER JOIN bill ON history.idBill = bill.idBill "
                        "INNER JOIN discount ON bill.idDiscount = discount.idDiscount "
                        "INNER JOIN vehicle ON vehicle.idVehicle = bill.idVehicle "
                        "INNER JOIN color ON color.idColor = vehicle.idColor "
                        "INNER JOIN tenant ON tenant.idTenant = vehicle.idTenant "
                        "INNER JOIN trademark ON trademark.idTrademark = vehicle.idTrademark "
                        "INNER JOIN user ON bill.idUser = user.idUser "
                        "WHERE LOWER(color_name) LIKE :color AND LOWER(trademark_name) LIKE :trademark");
            qry.bindValue(":color", QString("%%1%").arg(ui->lineEdit->text()));
            qry.bindValue(":trademark", QString("%%1%").arg(ui->lineEdit_2->text()));
            qry.exec();
            qry.first();
            ui->tableWidget->setRowCount(qry.value(0).toInt());

            qry.prepare("SELECT history.idHistory, history.date_start, history.date_finish, bill.bill_status, bill.bill_total, "
                        "bill.billing_time, discount.discount_size, trademark.trademark_name, tenant.tenant_name, color.color_name, bill.idBill "
                        " FROM history INNER JOIN bill ON history.idBill = bill.idBill "
                        "INNER JOIN discount ON bill.idDiscount = discount.idDiscount "
                        "INNER JOIN vehicle ON vehicle.idVehicle = bill.idVehicle "
                        "INNER JOIN color ON color.idColor = vehicle.idColor "
                        "INNER JOIN tenant ON tenant.idTenant = vehicle.idTenant "
                        "INNER JOIN trademark ON trademark.idTrademark = vehicle.idTrademark "
                        "INNER JOIN user ON bill.idUser = user.idUser "
                        "WHERE LOWER(color_name) LIKE :color AND LOWER(trademark_name) LIKE :trademark");
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
                        "WHERE user.user_email = :email AND color_name LIKE :color "
                        " AND trademark_name LIKE :trademark");
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
                        "WHERE user.user_email = :email AND lower(color_name) LIKE :color "
                        " AND trademark_name LIKE :trademark");
            qry.bindValue(":email", email);
            qry.bindValue(":color", QString("%%1%").arg(ui->lineEdit->text()));
            qry.bindValue(":trademark", QString("%%1%").arg(ui->lineEdit_2->text()));
            qry.exec();
        }

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
    } else {
        loadPageWorkerks();
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
    if (typeAccount == 0) {
        qry.prepare("SELECT idDiscount FROM discount WHERE code_name = :name");
        qry.bindValue(":name", ui->lineEdit_4->text());
        qry.exec();
        qry.first();
        int idCode = qry.value(0).toInt();
        if (idCode == 0) {
            idCode = 3;
        }
        if (idTemp != 0) {
            qry.prepare("INSERT INTO bill (bill_status, idUser, idDiscount, idVehicle) VALUES (0, (SELECT idUser FROM user WHERE user_email = :email), :idCode, :id)");
            qry.bindValue(":email", email);
            qry.bindValue(":id", idTemp);
            qry.bindValue(":idCode", idCode);
        } else {
            qry.prepare("INSERT INTO bill (bill_status, idUser, idDiscount, idVehicle) VALUES (0, (SELECT idUser FROM user WHERE user_email = :email), :idCode,"
                        " (SELECT idVehicle FROM machine WHERE state_number = :id))");
            qry.bindValue(":email", email);
            qry.bindValue(":id", ui->comboBox->currentText().split(" | ")[1]);
            qry.bindValue(":idCode", idCode);
        }
    } else {
        if (idTemp != 0) {
            qry.prepare("INSERT INTO repaired (repair_date, repair_cost, idVehicle, idWorker) VALUES (datetime(), 100, :idCode, (SELECT idUser FROM user WHERE user_email = :email))");
            qry.bindValue(":email", email);
            qry.bindValue(":idCode", idTemp);
        } else {
            qry.prepare("INSERT INTO repaired (repair_date, repair_cost, idVehicle, idWorker) VALUES (datetime(), 100, (SELECT idVehicle FROM machine WHERE state_number = :id), (SELECT idUser FROM user WHERE user_email = :email))");
            qry.bindValue(":email", email);
            qry.bindValue(":id", ui->comboBox->currentText().split(" | ")[1]);
        }
            QMessageBox *error = new QMessageBox(this);
            error->setWindowTitle("Успешно");
            error->setText("Вы успешно заправили");
            error->setIcon(QMessageBox::Information);
            error->setStandardButtons(QMessageBox::Ok);
            error->exec();
    }
    qry.exec();

    db.commit();
    on_pushButton_clicked();
}


void MainWindow::on_pushButton_4_clicked()
{
    QSqlQuery qry(db);
    qry.prepare("UPDATE history SET date_finish = datetime() WHERE idBill = "
                "(SELECT idBill FROM bill WHERE idUser = (SELECT idUser FROM user WHERE user_email = :email) AND billing_time is NULL)");
    qry.bindValue(":email", email);
    qry.exec();

    db.commit();
    on_pushButton_clicked();
    timer->stop();
}

void MainWindow::reloadTable()
{
    QSqlQuery qry(db);

    qry.prepare("SELECT trademark_name, color_name, ROUND(trademark_cost * (julianday(datetime()) - julianday(history.date_start)) * 24 * 60), ROUND((julianday(datetime()) - julianday(history.date_start)) * 24 * 60) FROM vehicle "
                "INNER JOIN trademark ON vehicle.idTrademark = trademark.idTrademark "
                "INNER JOIN color ON vehicle.idColor = color.idColor "
                "INNER JOIN bill ON vehicle.idVehicle = bill.idVehicle "
                "INNER JOIN history ON history.idBill = bill.idBill "
                "INNER JOIN user ON user.idUser = bill.idUser "
                "WHERE user.user_email = :email AND history.date_finish is NULL ");
    qry.bindValue(":email", email);
    qry.exec();
    qry.first();

    ui->tableWidget_3->setRowCount(1);
    ui->tableWidget_3->setItem(0, 0, new QTableWidgetItem(qry.value(0).toString()));
    ui->tableWidget_3->setItem(0, 1, new QTableWidgetItem(qry.value(1).toString()));
    ui->tableWidget_3->setItem(0, 2, new QTableWidgetItem(qry.value(2).toString()));
    ui->tableWidget_3->setItem(0, 3, new QTableWidgetItem(qry.value(3).toString() + " мин."));
}


void MainWindow::on_pushButton_5_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
}

void MainWindow::loadPage()
{
    if (typeAccount != 0) {
        ui->pushButton->setText("Ремонт/заправка т/c");
        ui->pushButton_2->setText("История заправок и ремонтов");
        ui->lineEdit_4->setHidden(true);
        ui->pushButton_3->setText("ЗАПРАВИТЬ!");
        ui->groupBox->setTitle("Машины доступные для взаимодействия");
    }
}

void MainWindow::loadPageWorkerks()
{
    ui->stackedWidget->setCurrentIndex(4);
    QSqlQuery qry(db);

    qry.prepare("SELECT COUNT(*) FROM repaired WHERE idWorker = (SELECT idUser FROM user WHERE user_email = :email AND type = 1)");
    qry.bindValue(":email", email);
    qry.exec();
    ui->tableWidget_4->setRowCount(qry.value(0).toInt());

    qry.prepare("SELECT * FROM repaired WHERE idWorker = (SELECT idUser FROM user WHERE user_email = :email AND type = 1)");
    qry.bindValue(":email", email);
    qry.exec();

    int i = 0;
    while (qry.next()) {
        ui->tableWidget_4->setItem(i, 0, new QTableWidgetItem(QString::number(qry.value(0).toInt())));
        ui->tableWidget_4->setItem(i, 1, new QTableWidgetItem(qry.value(1).toString()));
        ui->tableWidget_4->setItem(i, 2, new QTableWidgetItem(QString::number(qry.value(2).toInt())));
        ui->tableWidget_4->setItem(i, 3, new QTableWidgetItem(QString::number(qry.value(3).toInt())));
        ui->tableWidget_4->setItem(i, 4, new QTableWidgetItem(email));
        i++;
    }
}

