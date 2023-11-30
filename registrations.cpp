#include "registrations.h"
#include "ui_registrations.h"
#include <QDebug>
#include <QSqlError>

registrations::registrations(QSqlDatabase& db, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::registrations)
{
    ui->setupUi(this);
    this->db = db;
}

registrations::~registrations()
{
    delete ui;
}

QString registrations::returnLogin() {
    return ui->emailEdit->text();
}

void registrations::on_pushButton_clicked()
{
    QSqlQuery qry(db);

    qry.prepare("INSERT INTO user (user_email, user_telephone, user_name, driver_license, password) "
                "VALUES (:user_email, :user_telephone, :user_name, :driver_license, :password)");
    qry.bindValue(":user_email", ui->emailEdit->text());
    qry.bindValue(":user_telephone", ui->telephEdit->text());
    qry.bindValue(":user_name", ui->nameEdit->text());
    qry.bindValue(":driver_license", ui->licenseEdit->text());
    qry.bindValue(":password", ui->passEdit->text());
    qry.exec();

    db.commit();



    accept();
}

