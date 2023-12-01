#include "login.h"
#include "ui_login.h"
#include "registrations.h"
#include "mainwindow.h"
#include <QSqlDatabase>
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
    qDebug() << qry.value(0).toInt();
    if (qry.value(0).toInt() == 1) {
        email = ui->loginEdit->text();
        accept();
    }
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

