#include "mainwindow.h"
#include "login.h"
#include "QFile"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QFile f(":qdarkstyle/dark/darkstyle.qss");
    if (!f.exists())
    {
        qDebug() << "Unable to set stylesheet, file not found\n";
    }
    else
    {
        f.open(QFile::ReadOnly | QFile::Text);
        QTextStream ts(&f);
        a.setStyleSheet(ts.readAll());
    }

    MainWindow w;
    w.show();
    return a.exec();
}
