#pragma once
#include <string>
#include <unordered_map>
#include "DatabaseManager.h"
#include "Contactgui.h"

class PhoneBook {
public:
    unsigned int index;

    std::unordered_map<unsigned int, Contact> mainStorage;
    std::unordered_map<std::string, unsigned int> firstNameIndex;
    std::unordered_map<std::string, unsigned int> lastNameIndex;

    std::unordered_map<std::string, unsigned int> phoneWorkIndex;
    std::unordered_map<std::string, unsigned int> phoneHomeIndex;
    std::unordered_map<std::string, unsigned int> phoneOfficeIndex;

    std::unordered_map<std::string, unsigned int> emailIndex;

private: 
    std::string storageFile;
    bool m_useDatabase;

public:
    PhoneBook();
    PhoneBook(const PhoneBook& phoneBook);
    ~PhoneBook();

    int get_index();
    void set_index(int index);
    bool connectToDatabase(const QString& host = "localhost",
                           int port = 5432,
                           const QString& dbName = "phonebook_db",
                           const QString& user = "postgres",
                           const QString& password = "micheal1234");
    bool isUsingDatabase() const { return m_useDatabase; }
    void refreshCacheFromDatabase();  // Sync cache with database

public:
    void set_storage_file(const std::string& filename);
    const std::string& get_storage_file() const;
    bool save_to_file(const std::string& filename = "") const;
    bool load_from_file(const std::string& filename = "");
public:
    bool add_contact(const Contact& contact, std::string* error = nullptr);
    bool remove_contact(unsigned int id, std::string* error = nullptr);
    bool update_contact(unsigned int id, const Contact& updated, std::string* error = nullptr);
    bool get_contact(unsigned int id, Contact* out) const;

};
