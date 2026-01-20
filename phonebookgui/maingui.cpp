#include "mainwindow.h"
#include <QSqlDatabase>
#include <QDebug>
#include <QApplication>
#include <QSettings>
#include <QInputDialog>
#include <QMessageBox>
//void loadDatabaseConfig(PhoneBook& phoneBook);

int main(int argc, char *argv[])
{
    qDebug() << "Available drivers:" << QSqlDatabase::drivers();
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}

