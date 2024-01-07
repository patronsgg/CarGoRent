#include "registrations.h"
#include "ui_registrations.h"
#include "QMessageBox"
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

    qry.prepare("INSERT INTO user (user_email, user_telephone, user_name, driver_license, password, type) "
                "VALUES (:user_email, :user_telephone, :user_name, :driver_license, :password, 0)");
    qry.bindValue(":user_email", ui->emailEdit->text());
    qry.bindValue(":user_telephone", ui->telephEdit->text());
    qry.bindValue(":user_name", ui->nameEdit->text());
    qry.bindValue(":driver_license", ui->licenseEdit->text());
    qry.bindValue(":password", ui->passEdit->text());

    if (qry.exec() == false) {
        QMessageBox *error = new QMessageBox(this);
        error->setWindowTitle("Ошибка");
        error->setText("Неправильные данные");
        error->setIcon(QMessageBox::Critical);
        error->setStandardButtons(QMessageBox::Ok);
        error->exec();
        return;
    }

    db.commit();

    accept();
}

