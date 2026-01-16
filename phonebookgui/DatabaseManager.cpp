#ifndef DATABASEMANAGER_CPP
#define DATABASEMANAGER_CPP
#include "DatabaseManager.h"
#include <QSqlRecord>
#include <QDebug>
#include <QDate>

DatabaseManager& DatabaseManager::instance()
{
    static DatabaseManager instance;
    return instance;
}

DatabaseManager::DatabaseManager()
{
    m_db = QSqlDatabase::addDatabase("QPSQL");
}

DatabaseManager::~DatabaseManager()
{
    disconnect();
}

bool DatabaseManager::connect(const QString& host, int port,
                              const QString& dbName,
                              const QString& user,
                              const QString& password)
{
    m_db.setHostName(host);
    m_db.setPort(port);
    m_db.setDatabaseName(dbName);
    m_db.setUserName(user);
    m_db.setPassword(password);

    if (!m_db.open()) {
        m_lastError = m_db.lastError().text();
        qWarning() << "Database connection failed:" << m_lastError;
        return false;
    }

    qDebug() << "Connected to PostgreSQL database:" << dbName;
    return true;
}

void DatabaseManager::disconnect()
{
    if (m_db.isOpen()) {
        m_db.close();
        qDebug() << "Disconnected from database";
    }
}

bool DatabaseManager::isConnected() const
{
    return m_db.isOpen();
}

QString DatabaseManager::lastError() const
{
    return m_lastError;
}

bool DatabaseManager::createContact(const Contact& contact, unsigned int* outId)
{
    if (!isConnected()) {
        m_lastError = "Not connected to database";
        return false;
    }

    if (!beginTransaction()) {
        return false;
    }

    // Insert contact
    QSqlQuery query(m_db);
    query.prepare(
        "INSERT INTO contacts (first_name, middle_name, last_name, email, address, birthday) "
        "VALUES (:first_name, :middle_name, :last_name, :email, :address, :birthday) "
        "RETURNING id"
        );

    query.bindValue(":first_name", QString::fromStdString(contact.firstName));
    query.bindValue(":middle_name", QString::fromStdString(contact.middleName));
    query.bindValue(":last_name", QString::fromStdString(contact.lastName));
    query.bindValue(":email", QString::fromStdString(contact.email));
    query.bindValue(":address", QString::fromStdString(contact.address));

    // Convert birthday string to QDate
    if (!contact.birthday.empty()) {
        QDate date = QDate::fromString(QString::fromStdString(contact.birthday), "dd-MM-yyyy");
        query.bindValue(":birthday", date);
    } else {
        query.bindValue(":birthday", QVariant(QVariant::Date));
    }

    if (!query.exec()) {
        m_lastError = query.lastError().text();
        rollbackTransaction();
        return false;
    }

    // Get generated ID
    unsigned int contactId = 0;
    if (query.next()) {
        contactId = query.value(0).toUInt();
        if (outId) *outId = contactId;
    } else {
        rollbackTransaction();
        return false;
    }

    // Insert phones
    if (!insertPhones(contactId, contact.numbers)) {
        rollbackTransaction();
        return false;
    }

    return commitTransaction();
}

bool DatabaseManager::getContact(unsigned int id, Contact* out) const
{
    if (!isConnected() || !out) {
        return false;
    }

    QSqlQuery query(m_db);
    query.prepare(
        "SELECT id, first_name, middle_name, last_name, email, address, birthday "
        "FROM contacts WHERE id = :id"
        );
    query.bindValue(":id", id);

    if (!query.exec() || !query.next()) {
        return false;
    }

    *out = resultToContact(query);
    out->numbers = getPhonesForContact(id);

    return true;
}

bool DatabaseManager::updateContact(unsigned int id, const Contact& contact)
{
    if (!isConnected()) {
        m_lastError = "Not connected to database";
        return false;
    }

    if (!beginTransaction()) {
        return false;
    }

    // Update contact
    QSqlQuery query(m_db);
    query.prepare(
        "UPDATE contacts SET "
        "first_name = :first_name, "
        "middle_name = :middle_name, "
        "last_name = :last_name, "
        "email = :email, "
        "address = :address, "
        "birthday = :birthday "
        "WHERE id = :id"
        );

    query.bindValue(":id", id);
    query.bindValue(":first_name", QString::fromStdString(contact.firstName));
    query.bindValue(":middle_name", QString::fromStdString(contact.middleName));
    query.bindValue(":last_name", QString::fromStdString(contact.lastName));
    query.bindValue(":email", QString::fromStdString(contact.email));
    query.bindValue(":address", QString::fromStdString(contact.address));

    if (!contact.birthday.empty()) {
        QDate date = QDate::fromString(QString::fromStdString(contact.birthday), "dd-MM-yyyy");
        query.bindValue(":birthday", date);
    } else {
        query.bindValue(":birthday", QVariant(QVariant::Date));
    }

    if (!query.exec()) {
        m_lastError = query.lastError().text();
        rollbackTransaction();
        return false;
    }

    // Delete old phones and insert new ones
    if (!deletePhones(id) || !insertPhones(id, contact.numbers)) {
        rollbackTransaction();
        return false;
    }

    return commitTransaction();
}

bool DatabaseManager::deleteContact(unsigned int id)
{
    if (!isConnected()) {
        m_lastError = "Not connected to database";
        return false;
    }

    QSqlQuery query(m_db);
    query.prepare("DELETE FROM contacts WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        m_lastError = query.lastError().text();
        return false;
    }

    // Phones are automatically deleted due to CASCADE
    return query.numRowsAffected() > 0;
}

QList<QPair<unsigned int, Contact>> DatabaseManager::getAllContacts() const
{
    QList<QPair<unsigned int, Contact>> result;

    if (!isConnected()) {
        return result;
    }

    QSqlQuery query(m_db);
    query.prepare(
        "SELECT id, first_name, middle_name, last_name, email, address, birthday "
        "FROM contacts ORDER BY last_name, first_name"
        );

    if (!query.exec()) {
        return result;
    }

    while (query.next()) {
        unsigned int id = query.value("id").toUInt();
        Contact contact = resultToContact(query);
        contact.numbers = getPhonesForContact(id);
        result.append(qMakePair(id, contact));
    }

    return result;
}

QList<QPair<unsigned int, Contact>> DatabaseManager::searchByFirstName(const QString& name) const
{
    QList<QPair<unsigned int, Contact>> result;

    if (!isConnected()) {
        return result;
    }

    QSqlQuery query(m_db);
    query.prepare(
        "SELECT id, first_name, middle_name, last_name, email, address, birthday "
        "FROM contacts WHERE LOWER(first_name) LIKE LOWER(:name)"
        );
    query.bindValue(":name", "%" + name + "%");

    if (!query.exec()) {
        return result;
    }

    while (query.next()) {
        unsigned int id = query.value("id").toUInt();
        Contact contact = resultToContact(query);
        contact.numbers = getPhonesForContact(id);
        result.append(qMakePair(id, contact));
    }

    return result;
}

QList<QPair<unsigned int, Contact>> DatabaseManager::searchByLastName(const QString& name) const
{
    QList<QPair<unsigned int, Contact>> result;

    if (!isConnected()) {
        return result;
    }

    QSqlQuery query(m_db);
    query.prepare(
        "SELECT id, first_name, middle_name, last_name, email, address, birthday "
        "FROM contacts WHERE LOWER(last_name) LIKE LOWER(:name)"
        );
    query.bindValue(":name", "%" + name + "%");

    if (!query.exec()) {
        return result;
    }

    while (query.next()) {
        unsigned int id = query.value("id").toUInt();
        Contact contact = resultToContact(query);
        contact.numbers = getPhonesForContact(id);
        result.append(qMakePair(id, contact));
    }

    return result;
}

QList<QPair<unsigned int, Contact>> DatabaseManager::searchByEmail(const QString& email) const
{
    QList<QPair<unsigned int, Contact>> result;

    if (!isConnected()) {
        return result;
    }

    QSqlQuery query(m_db);
    query.prepare(
        "SELECT id, first_name, middle_name, last_name, email, address, birthday "
        "FROM contacts WHERE LOWER(email) LIKE LOWER(:email)"
        );
    query.bindValue(":email", "%" + email + "%");

    if (!query.exec()) {
        return result;
    }

    while (query.next()) {
        unsigned int id = query.value("id").toUInt();
        Contact contact = resultToContact(query);
        contact.numbers = getPhonesForContact(id);
        result.append(qMakePair(id, contact));
    }

    return result;
}

QList<QPair<unsigned int, Contact>> DatabaseManager::searchByPhone(const QString& phone) const
{
    QList<QPair<unsigned int, Contact>> result;

    if (!isConnected()) {
        return result;
    }

    QSqlQuery query(m_db);
    query.prepare(
        "SELECT DISTINCT c.id, c.first_name, c.middle_name, c.last_name, c.email, c.address, c.birthday "
        "FROM contacts c "
        "JOIN phones p ON c.id = p.contact_id "
        "WHERE p.phone_number LIKE :phone"
        );
    query.bindValue(":phone", "%" + phone + "%");

    if (!query.exec()) {
        return result;
    }

    while (query.next()) {
        unsigned int id = query.value("id").toUInt();
        Contact contact = resultToContact(query);
        contact.numbers = getPhonesForContact(id);
        result.append(qMakePair(id, contact));
    }

    return result;
}

int DatabaseManager::getContactCount() const
{
    if (!isConnected()) {
        return 0;
    }

    QSqlQuery query("SELECT COUNT(*) FROM contacts", m_db);
    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    }
    return 0;
}

bool DatabaseManager::contactExists(unsigned int id) const
{
    if (!isConnected()) {
        return false;
    }

    QSqlQuery query(m_db);
    query.prepare("SELECT 1 FROM contacts WHERE id = :id");
    query.bindValue(":id", id);

    return query.exec() && query.next();
}

bool DatabaseManager::beginTransaction()
{
    if (!m_db.transaction()) {
        m_lastError = m_db.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::commitTransaction()
{
    if (!m_db.commit()) {
        m_lastError = m_db.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::rollbackTransaction()
{
    if (!m_db.rollback()) {
        m_lastError = m_db.lastError().text();
        return false;
    }
    return true;
}

Contact DatabaseManager::resultToContact(const QSqlQuery& query) const
{
    Contact contact;

    contact.firstName = query.value("first_name").toString().toStdString();
    contact.middleName = query.value("middle_name").toString().toStdString();
    contact.lastName = query.value("last_name").toString().toStdString();
    contact.email = query.value("email").toString().toStdString();
    contact.address = query.value("address").toString().toStdString();

    QDate birthday = query.value("birthday").toDate();
    if (birthday.isValid()) {
        contact.birthday = birthday.toString("dd-MM-yyyy").toStdString();
    }

    return contact;
}

bool DatabaseManager::insertPhones(unsigned int contactId, const Phone& phones)
{
    QSqlQuery query(m_db);
    query.prepare(
        "INSERT INTO phones (contact_id, phone_type, phone_number) "
        "VALUES (:contact_id, :phone_type, :phone_number)"
        );

    auto insertPhone = [&](const std::string& number, const QString& type) -> bool {
        if (number.empty()) return true;

        query.bindValue(":contact_id", contactId);
        query.bindValue(":phone_type", type);
        query.bindValue(":phone_number", QString::fromStdString(number));

        if (!query.exec()) {
            m_lastError = query.lastError().text();
            return false;
        }
        return true;
    };

    if (!insertPhone(phones.number1, "work")) return false;
    if (!insertPhone(phones.number2, "home")) return false;
    if (!insertPhone(phones.number3, "office")) return false;

    return true;
}

bool DatabaseManager::deletePhones(unsigned int contactId)
{
    QSqlQuery query(m_db);
    query.prepare("DELETE FROM phones WHERE contact_id = :contact_id");
    query.bindValue(":contact_id", contactId);

    if (!query.exec()) {
        m_lastError = query.lastError().text();
        return false;
    }
    return true;
}

Phone DatabaseManager::getPhonesForContact(unsigned int contactId) const
{
    Phone phones;

    QSqlQuery query(m_db);
    query.prepare(
        "SELECT phone_type, phone_number FROM phones WHERE contact_id = :contact_id"
        );
    query.bindValue(":contact_id", contactId);

    if (!query.exec()) {
        return phones;
    }

    while (query.next()) {
        QString type = query.value("phone_type").toString();
        std::string number = query.value("phone_number").toString().toStdString();

        if (type == "work") {
            phones.number1 = number;
        } else if (type == "home") {
            phones.number2 = number;
        } else if (type == "office") {
            phones.number3 = number;
        }
    }

    return phones;
}
#endif // DATABASEMANAGER_CPP
