#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QString>
#include <QVariant>
#include <memory>
#include "Contactgui.h"

class DatabaseManager
{
public:
    static DatabaseManager& instance();

    // Connection management
    bool connect(const QString& host = "localhost",
                 int port = 5432,
                 const QString& dbName = "phonebook_db",
                 const QString& user = "postgres",
                 const QString& password = "micheal1234");

    void disconnect();
    bool isConnected() const;
    QString lastError() const;

    // CRUD operations
    bool createContact(const Contact& contact, unsigned int* outId = nullptr);
    bool getContact(unsigned int id, Contact* out) const;
    bool updateContact(unsigned int id, const Contact& contact);
    bool deleteContact(unsigned int id);

    // Search operations
    QList<QPair<unsigned int, Contact>> getAllContacts() const;
    QList<QPair<unsigned int, Contact>> searchByFirstName(const QString& name) const;
    QList<QPair<unsigned int, Contact>> searchByLastName(const QString& name) const;
    QList<QPair<unsigned int, Contact>> searchByEmail(const QString& email) const;
    QList<QPair<unsigned int, Contact>> searchByPhone(const QString& phone) const;

    // Utility
    int getContactCount() const;
    bool contactExists(unsigned int id) const;

    // Transaction support
    bool beginTransaction();
    bool commitTransaction();
    bool rollbackTransaction();

private:
    DatabaseManager();
    ~DatabaseManager();
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    QSqlDatabase m_db;
    QString m_lastError;

    // Helper methods
    bool executeQuery(QSqlQuery& query) const;
    Contact resultToContact(const QSqlQuery& query) const;
    bool insertPhones(unsigned int contactId, const Phone& phones);
    bool deletePhones(unsigned int contactId);
    Phone getPhonesForContact(unsigned int contactId) const;
};

#endif // DATABASEMANAGER_H
