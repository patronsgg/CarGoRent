#include "login.h"
#include "ui_login.h"
#include "registrations.h"
#include "mainwindow.h"
#include <QSqlDatabase>
#include "QMessageBox"
#include <QDebug>

login::login(QSqlDatabase db, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::login)
{
    ui->setupUi(this);
    this->db = db;
}

login::~login()
{
    delete ui;
}

void login::on_loginButton_clicked()
{
    QSqlQuery qry(db);
    qry.prepare("SELECT COUNT(*) FROM user WHERE (user_email = :login OR user_telephone = :login) AND password = :pass");
    qry.bindValue(":login", ui->loginEdit->text());
    qry.bindValue(":pass", ui->passwordEdit->text());
    qry.exec();
    qry.first();
    if (qry.value(0).toInt() == 1) {
        email = ui->loginEdit->text();
        qry.prepare("SELECT type FROM user WHERE user_email = :login");
        qry.bindValue(":login", ui->loginEdit->text());
        qry.exec();
        qry.first();
        type = qry.value(0).toInt();
        accept();
        return;
    }

    QMessageBox *error = new QMessageBox(this);
    error->setWindowTitle("Ошибка");
    error->setText("Неправильные данные");
    error->setIcon(QMessageBox::Critical);
    error->setStandardButtons(QMessageBox::Ok);
    error->exec();

}


void login::on_registerButton_clicked()
{
    registrations reg(db);
    reg.setModal(true);
    reg.setWindowTitle(tr("CarGoRent"));
    switch(reg.exec()) {
    case QDialog::Accepted:
        ui->loginEdit->setText(reg.returnLogin());
        break;
    }
}

