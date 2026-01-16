#include "mainwindow.h"
#include <QSqlDatabase>
#include <QDebug>
#include <QApplication>
#include <QSettings>
#include <QInputDialog>
#include <QMessageBox>
void loadDatabaseConfig(PhoneBook& phoneBook);

int main(int argc, char *argv[])
{
    qDebug() << "Available drivers:" << QSqlDatabase::drivers();
    QApplication a(argc, argv);
    // Create phonebook instance
    PhoneBook phoneBook;

    // Load database configuration
    loadDatabaseConfig(phoneBook);

    MainWindow w;
    w.show();
    return a.exec();
}
void loadDatabaseConfig(PhoneBook& phoneBook) {
    QSettings settings("db_config.ini", QSettings::IniFormat);

    QString host = settings.value("Database/host", "localhost").toString();
    int port = settings.value("Database/port", 5432).toInt();
    QString dbName = settings.value("Database/database", "phonebook_db").toString();
    QString user = settings.value("Database/username", "postgres").toString();

    // Password from environment variable for security
    QString password = qgetenv("PHONEBOOK_DB_PASSWORD");

    if (password.isEmpty()) {
        // Prompt user for password
        bool ok;
        password = QInputDialog::getText(nullptr, "Database Password",
                                         "Enter database password:",
                                         QLineEdit::Password,
                                         "", &ok);
        if (!ok) return;
    }

    phoneBook.connectToDatabase(host, port, dbName, user, password);
}
