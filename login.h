#ifndef LOGIN_H
#define LOGIN_H

#include <QDialog>

#include <QSqlDatabase>

namespace Ui {
class login;
}

class login : public QDialog
{
    Q_OBJECT

public:
    explicit login(QSqlDatabase db, QWidget *parent = nullptr);
    ~login();
    QString returnEmail() { return email; }

private slots:
    void on_loginButton_clicked();

    void on_registerButton_clicked();

private:
    Ui::login *ui;
    QSqlDatabase db;
    QString email;
};

#endif // LOGIN_H
