#pragma once
#include <string>
#include <unordered_map>
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

public:
    PhoneBook();
    PhoneBook(const PhoneBook& phoneBook);
    ~PhoneBook();

    int get_index();
    void set_index(int index);

public:
    void set_storage_file(const std::string& filename);
    const std::string& get_storage_file() const;
    bool save_to_file(const std::string& filename = "") const;
    bool load_from_file(const std::string& filename = "");

public:
    void contact_creation_menu();
    Contact contact_search_menu();
    void edit_contact();
    void delete_contact();
    void contact_sort_menu();

private:
    void create_contact(Contact contact);
    Contact search(char method, const std::string& value);
    void edit_contact_fields(PhoneBook& book, unsigned int id);
    void delete_contact_impl(PhoneBook& book, unsigned int id);
    void list_sorted_contacts(char method);
public:
    bool add_contact(const Contact& contact, std::string* error = nullptr);
    bool remove_contact(unsigned int id, std::string* error = nullptr);
    bool update_contact(unsigned int id, const Contact& updated, std::string* error = nullptr);
    bool get_contact(unsigned int id, Contact* out) const;

};
