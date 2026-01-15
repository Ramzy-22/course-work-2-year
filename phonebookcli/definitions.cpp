#include "PhoneBook.h"
#include "Checkers.h"
#include <iostream>
#include <limits>
#include <regex>
#include <fstream>
#include <sstream>
#include <iomanip>

PhoneBook::PhoneBook() : index(0), storageFile("phonebook.db")
{
    // Best-effort load: if the file does not exist or is invalid,
    // the phone book starts empty.
    (void)load_from_file();
}

PhoneBook::PhoneBook(const PhoneBook& other) = default;

PhoneBook::~PhoneBook()
{
    // Best-effort save on shutdown (changes are also saved after create/edit/delete).
    (void)save_to_file();
}

int PhoneBook::get_index() { return static_cast<int>(index); }
void PhoneBook::set_index(int idx) { index = static_cast<unsigned int>(idx); }

void PhoneBook::set_storage_file(const std::string& filename)
{
    storageFile = filename.empty() ? std::string("phonebook.db") : filename;
}

const std::string& PhoneBook::get_storage_file() const
{
    return storageFile;
}

bool PhoneBook::save_to_file(const std::string& filename) const
{
    const std::string file = filename.empty() ? storageFile : filename;
    std::ofstream out(file, std::ios::trunc);
    if (!out.is_open()) {
        return false;
    }

    // Simple, robust text format. One contact per line with std::quoted strings.
    // Header
    out << "PHONEBOOK_V1\n";
    out << index << "\n";
    out << mainStorage.size() << "\n";

    for (const auto& pair : mainStorage) {
        const unsigned int id = pair.first;
        const Contact& c = pair.second;
        out << id << ' '
            << std::quoted(c.firstName) << ' '
            << std::quoted(c.middleName) << ' '
            << std::quoted(c.lastName) << ' '
            << std::quoted(c.numbers.number1) << ' '
            << std::quoted(c.numbers.number2) << ' '
            << std::quoted(c.numbers.number3) << ' '
            << std::quoted(c.email) << ' '
            << std::quoted(c.address) << ' '
            << std::quoted(c.birthday)
            << "\n";
    }

    return static_cast<bool>(out);
}

bool PhoneBook::load_from_file(const std::string& filename)
{
    const std::string file = filename.empty() ? storageFile : filename;
    std::ifstream in(file);
    if (!in.is_open()) {
        return false;
    }

    std::string header;
    if (!std::getline(in, header)) {
        return false;
    }
    if (header != "PHONEBOOK_V1") {
        // Unknown format
        return false;
    }

    unsigned int fileIndex = 0;
    std::size_t count = 0;
    {
        std::string line;
        if (!std::getline(in, line)) return false;
        std::istringstream iss(line);
        if (!(iss >> fileIndex)) return false;
        if (!std::getline(in, line)) return false;
        std::istringstream iss2(line);
        if (!(iss2 >> count)) return false;
    }

    // Reset current state
    mainStorage.clear();
    firstNameIndex.clear();
    lastNameIndex.clear();
    phoneWorkIndex.clear();
    phoneHomeIndex.clear();
    phoneOfficeIndex.clear();
    emailIndex.clear();
    index = 0;

    unsigned int maxId = 0;
    std::string recordLine;
    std::size_t loaded = 0;

    while (std::getline(in, recordLine)) {
        if (recordLine.empty()) continue;
        std::istringstream iss(recordLine);

        unsigned int id = 0;
        Contact c;
        if (!(iss >> id
            >> std::quoted(c.firstName)
            >> std::quoted(c.middleName)
            >> std::quoted(c.lastName)
            >> std::quoted(c.numbers.number1)
            >> std::quoted(c.numbers.number2)
            >> std::quoted(c.numbers.number3)
            >> std::quoted(c.email)
            >> std::quoted(c.address)
            >> std::quoted(c.birthday))) {
            // Skip malformed line
            continue;
        }

        mainStorage[id] = c;
        maxId = std::max(maxId, id);

        // Rebuild indices (same behavior as your create_contact logic).
        if (!c.firstName.empty()) firstNameIndex[c.firstName] = id;
        if (!c.lastName.empty())  lastNameIndex[c.lastName] = id;
        if (!c.numbers.number1.empty()) phoneWorkIndex[c.numbers.number1] = id;
        if (!c.numbers.number2.empty()) phoneHomeIndex[c.numbers.number2] = id;
        if (!c.numbers.number3.empty()) phoneOfficeIndex[c.numbers.number3] = id;
        if (!c.email.empty()) emailIndex[c.email] = id;

        ++loaded;
        if (count != 0 && loaded >= count) {
            // If the file says how many contacts exist, stop after that many.
            break;
        }
    }

    // Keep index in sync so new IDs do not collide.
    index = std::max(fileIndex, maxId);
    return true;
}

void PhoneBook::create_contact(Contact contact)
{
    bool ok = true;

    // -------- REQUIRED FOR CREATING A CONTACT --------
    // 1) First name + Last name
    if (!isValidName(contact.firstName) || !isValidName(contact.lastName)) {
        ok = false;
    }

    // 2) Email
    if (!isValidEmail(contact.email)) {
        ok = false;
    }

    // 3) At least ONE valid phone from:
    //    numbers.number1 -> work phone
    //    numbers.number2 -> home phone
    //    numbers.number3 -> office phone
    bool hasAtLeastOnePhone = false;
    bool allPhonesValid = true;

    auto checkPhoneField = [&](const std::string& p) {
        if (!p.empty()) {
            if (!isValidPhone(p)) {
                allPhonesValid = false;
            }
            else {
                hasAtLeastOnePhone = true;
            }
        }
        };

    checkPhoneField(contact.numbers.number1); // work
    checkPhoneField(contact.numbers.number2); // home
    checkPhoneField(contact.numbers.number3); // office

    if (!allPhonesValid || !hasAtLeastOnePhone) {
        ok = false;
    }

    if (!ok) {
        std::cout << "Missing required fields (first, last, email, one phone)." << std::endl;
        return;
    }

    // -------- ADDITIONAL FIELDS (OPTIONAL, BUT VALIDATED IF PRESENT) --------

    // Middle name: optional
    if (!contact.middleName.empty() && !isValidName(contact.middleName)) {
        std::cout << "Invalid middle name." << std::endl;
        return;
    }

    // Address: optional – if not empty, must have at least one non-space
    static const std::regex addressPattern(R"(^.*\S.*$)");
    if (!contact.address.empty() &&
        !std::regex_match(contact.address, addressPattern)) {
        std::cout << "Invalid address (must contain at least one non-space character)." << std::endl;
        return;
    }

    // Birthday: optional – if not empty, must be a valid past date
    if (!contact.birthday.empty() && !isValidBirthday(contact.birthday)) {
        std::cout << "Invalid birthday." << std::endl;
        return;
    }

    // -------- STORE CONTACT AND UPDATE INDICES --------

    unsigned int newId = ++index;  // index starts at 0 in constructor, so first is 1
    mainStorage[newId] = contact;

    // Name indices
    firstNameIndex[contact.firstName] = newId;
    lastNameIndex[contact.lastName] = newId;

    // Phone indices using your mapping:
    // number1 -> work, number2 -> home, number3 -> office
    if (!contact.numbers.number1.empty()) {
        phoneWorkIndex[contact.numbers.number1] = newId;
    }
    if (!contact.numbers.number2.empty()) {
        phoneHomeIndex[contact.numbers.number2] = newId;
    }
    if (!contact.numbers.number3.empty()) {
        phoneOfficeIndex[contact.numbers.number3] = newId;
    }

    // Email index
    emailIndex[contact.email] = newId;

    std::cout << "Contact created successfully" << std::endl;

    // Persist immediately so data survives program restart.
    if (!save_to_file()) {
        std::cout << "Warning: could not save phone book to file ('" << storageFile << "').\n";
    }
}


Contact PhoneBook::search(char method, const std::string& value)
{
    // Re-validate the search value based on the method
    switch (method) {
    case '1': // first name
    case '2': // last name
        if (!isValidName(value)) {
            std::cout << "Search value is not a valid name.\n";
            return Contact{};
        }
        break;

    case '3': // work phone
    case '4': // home phone
    case '5': // office phone
        if (!isValidPhone(value)) {
            std::cout << "Search value is not a valid phone number.\n";
            return Contact{};
        }
        break;

    case '6': // email
        if (!isValidEmail(value)) {
            std::cout << "Search value is not a valid email.\n";
            return Contact{};
        }
        break;

    default:
        std::cout << "Unknown search method.\n";
        return Contact{};
    }

    // Now we know the value is valid for this method.
    unsigned int id = 0;
    bool found = false;

    switch (method) {
    case '1': { // first name
        auto it = firstNameIndex.find(value);
        if (it != firstNameIndex.end()) {
            id = it->second;
            found = true;
        }
        break;
    }
    case '2': { // last name
        auto it = lastNameIndex.find(value);
        if (it != lastNameIndex.end()) {
            id = it->second;
            found = true;
        }
        break;
    }
    case '3': { // work phone
        auto it = phoneWorkIndex.find(value);
        if (it != phoneWorkIndex.end()) {
            id = it->second;
            found = true;
        }
        break;
    }
    case '4': { // home phone
        auto it = phoneHomeIndex.find(value);
        if (it != phoneHomeIndex.end()) {
            id = it->second;
            found = true;
        }
        break;
    }
    case '5': { // office phone
        auto it = phoneOfficeIndex.find(value);
        if (it != phoneOfficeIndex.end()) {
            id = it->second;
            found = true;
        }
        break;
    }
    case '6': { // email
        auto it = emailIndex.find(value);
        if (it != emailIndex.end()) {
            id = it->second;
            found = true;
        }
        break;
    }
    }

    if (!found) {
        std::cout << "No contact found for the given search value.\n";
        return Contact{};
    }

    // Retrieve contact from mainStorage by id
    auto itMain = mainStorage.find(id);
    if (itMain != mainStorage.end()) {
        return itMain->second;
    }
    else {
        std::cout << "Internal error: contact not found in main storage.\n";
        return Contact{};
    }
}

 void PhoneBook::edit_contact_fields(PhoneBook& book, unsigned int id)
{
    auto itMain = book.mainStorage.find(id);
    if (itMain == book.mainStorage.end()) {
        std::cout << "Internal error: contact not found.\n";
        return;
    }

    Contact& contact = itMain->second;

    while (true) {
        std::cout << "\n===== EDIT MENU for ID " << id << " =====\n";
        std::cout << "1) First name   (current: " << contact.firstName << ")\n";
        std::cout << "2) Last name    (current: " << contact.lastName << ")\n";
        std::cout << "3) Middle name  (current: " << contact.middleName << ")\n";
        std::cout << "4) Email        (current: " << contact.email << ")\n";
        std::cout << "5) Work phone   (current: " << contact.numbers.number1 << ")\n";
        std::cout << "6) Home phone   (current: " << contact.numbers.number2 << ")\n";
        std::cout << "7) Office phone (current: " << contact.numbers.number3 << ")\n";
        std::cout << "8) Address      (current: " << contact.address << ")\n";
        std::cout << "9) Birthday     (current: " << contact.birthday << ")\n";
        std::cout << "0) Finish editing\n";
        std::cout << "Choose field to edit: ";

        char choice;
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (choice == '0') {
            std::cout << "Finished editing contact.\n";
            break;
        }

        std::string input;

        switch (choice) {
        case '1': { // First name
            std::string oldVal = contact.firstName;
            std::cout << "Enter new FIRST name (leave empty to keep '" << oldVal << "'): ";
            std::getline(std::cin, input);

            while (!input.empty() && !isValidName(input)) {
                std::cout << "Invalid name. Try again (or empty to keep current): ";
                std::getline(std::cin, input);
            }

            if (!input.empty() && input != oldVal) {
                auto itIndex = book.firstNameIndex.find(oldVal);
                if (itIndex != book.firstNameIndex.end() && itIndex->second == id) {
                    book.firstNameIndex.erase(itIndex);
                }
                book.firstNameIndex[input] = id;
                contact.firstName = input;
            }
            break;
        }
        case '2': { // Last name
            std::string oldVal = contact.lastName;
            std::cout << "Enter new LAST name (leave empty to keep '" << oldVal << "'): ";
            std::getline(std::cin, input);

            while (!input.empty() && !isValidName(input)) {
                std::cout << "Invalid name. Try again (or empty to keep current): ";
                std::getline(std::cin, input);
            }

            if (!input.empty() && input != oldVal) {
                auto itIndex = book.lastNameIndex.find(oldVal);
                if (itIndex != book.lastNameIndex.end() && itIndex->second == id) {
                    book.lastNameIndex.erase(itIndex);
                }
                book.lastNameIndex[input] = id;
                contact.lastName = input;
            }
            break;
        }
        case '3': { // Middle name (no index map)
            std::string oldVal = contact.middleName;
            std::cout << "Enter new MIDDLE name (leave empty to keep '" << oldVal << "'): ";
            std::getline(std::cin, input);

            while (!input.empty() && !isValidName(input)) {
                std::cout << "Invalid name. Try again (or empty to keep current): ";
                std::getline(std::cin, input);
            }

            if (!input.empty()) {
                contact.middleName = input;
            }
            break;
        }
        case '4': { // Email
            std::string oldVal = contact.email;
            std::cout << "Enter new EMAIL (leave empty to keep '" << oldVal << "'): ";
            std::getline(std::cin, input);

            while (!input.empty() && !isValidEmail(input)) {
                std::cout << "Invalid email. Try again (or empty to keep current): ";
                std::getline(std::cin, input);
            }

            if (!input.empty() && input != oldVal) {
                auto itIndex = book.emailIndex.find(oldVal);
                if (itIndex != book.emailIndex.end() && itIndex->second == id) {
                    book.emailIndex.erase(itIndex);
                }
                book.emailIndex[input] = id;
                contact.email = input;
            }
            break;
        }
        case '5': { // Work phone (numbers.number1)
            std::string oldVal = contact.numbers.number1;
            std::cout << "Enter new WORK phone (leave empty to keep '" << oldVal << "'): ";
            std::getline(std::cin, input);

            while (!input.empty() && !isValidPhone(input)) {
                std::cout << "Invalid phone. Try again (or empty to keep current): ";
                std::getline(std::cin, input);
            }

            if (!input.empty() && input != oldVal) {
                if (!oldVal.empty()) {
                    auto itIndex = book.phoneWorkIndex.find(oldVal);
                    if (itIndex != book.phoneWorkIndex.end() && itIndex->second == id) {
                        book.phoneWorkIndex.erase(itIndex);
                    }
                }
                book.phoneWorkIndex[input] = id;
                contact.numbers.number1 = input;
            }
            break;
        }
        case '6': { // Home phone (numbers.number2)
            std::string oldVal = contact.numbers.number2;
            std::cout << "Enter new HOME phone (leave empty to keep '" << oldVal << "'): ";
            std::getline(std::cin, input);

            while (!input.empty() && !isValidPhone(input)) {
                std::cout << "Invalid phone. Try again (or empty to keep current): ";
                std::getline(std::cin, input);
            }

            if (!input.empty() && input != oldVal) {
                if (!oldVal.empty()) {
                    auto itIndex = book.phoneHomeIndex.find(oldVal);
                    if (itIndex != book.phoneHomeIndex.end() && itIndex->second == id) {
                        book.phoneHomeIndex.erase(itIndex);
                    }
                }
                book.phoneHomeIndex[input] = id;
                contact.numbers.number2 = input;
            }
            break;
        }
        case '7': { // Office phone (numbers.number3)
            std::string oldVal = contact.numbers.number3;
            std::cout << "Enter new OFFICE phone (leave empty to keep '" << oldVal << "'): ";
            std::getline(std::cin, input);

            while (!input.empty() && !isValidPhone(input)) {
                std::cout << "Invalid phone. Try again (or empty to keep current): ";
                std::getline(std::cin, input);
            }

            if (!input.empty() && input != oldVal) {
                if (!oldVal.empty()) {
                    auto itIndex = book.phoneOfficeIndex.find(oldVal);
                    if (itIndex != book.phoneOfficeIndex.end() && itIndex->second == id) {
                        book.phoneOfficeIndex.erase(itIndex);
                    }
                }
                book.phoneOfficeIndex[input] = id;
                contact.numbers.number3 = input;
            }
            break;
        }
        case '8': { // Address (no index map)
            std::string oldVal = contact.address;
            static const std::regex addressPattern(R"(^.*\S.*$)");

            std::cout << "Enter new ADDRESS (leave empty to keep '" << oldVal << "'): ";
            std::getline(std::cin, input);

            while (!input.empty() && !std::regex_match(input, addressPattern)) {
                std::cout << "Invalid address. Must contain at least one non-space.\n"
                    "Try again (or empty to keep current): ";
                std::getline(std::cin, input);
            }

            if (!input.empty()) {
                contact.address = input;
            }
            break;
        }
        case '9': { // Birthday (no index map)
            std::string oldVal = contact.birthday;
            std::cout << "Enter new BIRTHDAY (dd-mm-yyyy, leave empty to keep '" << oldVal << "'): ";
            std::getline(std::cin, input);

            while (!input.empty() && !isValidBirthday(input)) {
                std::cout << "Invalid birthday. Use dd-mm-yyyy, must be a past valid date.\n"
                    "Try again (or empty to keep current): ";
                std::getline(std::cin, input);
            }

            if (!input.empty()) {
                contact.birthday = input;
            }
            break;
        }
        default:
            std::cout << "Unknown choice. Try again.\n";
            break;
        }
    }

    // Persist edits immediately.
    (void)book.save_to_file();
}

void PhoneBook::delete_contact_impl(PhoneBook& book, unsigned int id)
{
    auto itMain = book.mainStorage.find(id);
    if (itMain == book.mainStorage.end()) {
        std::cout << "Internal error: contact not found in main storage.\n";
        return;
    }

    // Copy contact before erasing, so we still know the keys to remove from indices
    Contact contact = itMain->second;

    // Remove from main storage
    book.mainStorage.erase(itMain);

    // Helper lambda to erase key from a map if it belongs to this id
    auto eraseKey = [id](auto& mp, const std::string& key) {
        if (key.empty()) return;
        auto it = mp.find(key);
        if (it != mp.end() && it->second == id) {
            mp.erase(it);
        }
        };

    // Name indices
    eraseKey(book.firstNameIndex, contact.firstName);
    eraseKey(book.lastNameIndex, contact.lastName);

    // Phone indices: number1=work, number2=home, number3=office
    eraseKey(book.phoneWorkIndex, contact.numbers.number1);
    eraseKey(book.phoneHomeIndex, contact.numbers.number2);
    eraseKey(book.phoneOfficeIndex, contact.numbers.number3);

    // Email index
    eraseKey(book.emailIndex, contact.email);

    std::cout << "Contact deleted successfully.\n";

    // Persist immediately.
    (void)book.save_to_file();
}
void PhoneBook::list_sorted_contacts(char method)
{
    if (mainStorage.empty()) {
        std::cout << "Phone book is empty. Nothing to list.\n";
        return;
    }

    // Collect all IDs from mainStorage
    std::vector<unsigned int> ids;
    ids.reserve(mainStorage.size());
    for (const auto& pair : mainStorage) {
        ids.push_back(pair.first);
    }

    // Decide how to compare based on method:
    // '1' -> first name, '2' -> last name
    if (method == '1') {
        // Sort by first name ascending, tie-break by ID
        std::sort(ids.begin(), ids.end(),
            [this](unsigned int a, unsigned int b) {
                const std::string& na = mainStorage.at(a).firstName;
                const std::string& nb = mainStorage.at(b).firstName;
                if (na != nb)
                    return na < nb;   // ascending lexicographical order
                return a < b;         // tie-break: smaller ID first
            }
        );

        std::cout << "==== CONTACTS SORTED BY FIRST NAME (ASC) ====\n";

    }
    else if (method == '2') {
        // Sort by last name ascending, tie-break by ID
        std::sort(ids.begin(), ids.end(),
            [this](unsigned int a, unsigned int b) {
                const std::string& na = mainStorage.at(a).lastName;
                const std::string& nb = mainStorage.at(b).lastName;
                if (na != nb)
                    return na < nb;   // ascending lexicographical order
                return a < b;         // tie-break: smaller ID first
            }
        );

        std::cout << "==== CONTACTS SORTED BY LAST NAME (ASC) ====\n";

    }
    else {
        std::cout << "Unknown sort method. Use '1' for first name or '2' for last name.\n";
        return;
    }

    // Print all contacts in the sorted order
    for (unsigned int id : ids) {
        std::cout << "\n[ID: " << id << "]\n";
        mainStorage.at(id).print_contact();  // uses your Contact::print_contact()
    }
}
