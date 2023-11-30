#ifndef REGISTRATIONS_H
#define REGISTRATIONS_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDialog>

namespace Ui {
class registrations;
}

class registrations : public QDialog
{
    Q_OBJECT

public:
    explicit registrations(QSqlDatabase& db, QWidget *parent = nullptr);
    ~registrations();
    QString returnLogin();

private slots:
    void on_pushButton_clicked();

private:
    Ui::registrations *ui;
    QSqlDatabase db;

};

#endif // REGISTRATIONS_H
