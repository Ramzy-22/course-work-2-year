#include "PhoneBookgui.h"
#include "Checkersgui.h"
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>


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
    const std::string fileStd = filename.empty() ? storageFile : filename;
    QFile f(QString::fromStdString(fileStd));
    if (!f.open(QIODevice::WriteOnly)) return false;

    QJsonObject root;
    root["version"] = 1;
    root["index"] = static_cast<int>(index);

    QJsonArray contacts;
    for (const auto& pair : mainStorage) {
        const unsigned int id = pair.first;
        const Contact& c = pair.second;

        QJsonObject o;
        o["id"] = static_cast<int>(id);
        o["firstName"] = QString::fromStdString(c.firstName);
        o["middleName"] = QString::fromStdString(c.middleName);
        o["lastName"] = QString::fromStdString(c.lastName);
        o["email"] = QString::fromStdString(c.email);
        o["address"] = QString::fromStdString(c.address);
        o["birthday"] = QString::fromStdString(c.birthday);

        QJsonObject phones;
        phones["work"] = QString::fromStdString(c.numbers.number1);
        phones["home"] = QString::fromStdString(c.numbers.number2);
        phones["office"] = QString::fromStdString(c.numbers.number3);
        o["phones"] = phones;

        contacts.append(o);
    }

    root["contacts"] = contacts;

    QJsonDocument doc(root);
    f.write(doc.toJson(QJsonDocument::Indented));
    return true;
}

bool PhoneBook::load_from_file(const std::string& filename)
{
    const std::string fileStd = filename.empty() ? storageFile : filename;
    QFile f(QString::fromStdString(fileStd));
    if (!f.open(QIODevice::ReadOnly)) return false;

    const QByteArray data = f.readAll();
    QJsonParseError pe{};
    QJsonDocument doc = QJsonDocument::fromJson(data, &pe);
    if (pe.error != QJsonParseError::NoError) return false;
    if (!doc.isObject()) return false;

    QJsonObject root = doc.object();
    if (!root.contains("contacts") || !root.value("contacts").isArray()) return false;

    // Reset
    mainStorage.clear();
    firstNameIndex.clear();
    lastNameIndex.clear();
    phoneWorkIndex.clear();
    phoneHomeIndex.clear();
    phoneOfficeIndex.clear();
    emailIndex.clear();

    unsigned int maxId = 0;

    const QJsonArray contacts = root.value("contacts").toArray();
    for (const QJsonValue& v : contacts) {
        if (!v.isObject()) continue;
        QJsonObject o = v.toObject();

        unsigned int id = static_cast<unsigned int>(o.value("id").toInt(0));
        if (id == 0) continue;

        Contact c;
        c.firstName  = o.value("firstName").toString().toStdString();
        c.middleName = o.value("middleName").toString().toStdString();
        c.lastName   = o.value("lastName").toString().toStdString();
        c.email      = o.value("email").toString().toStdString();
        c.address    = o.value("address").toString().toStdString();
        c.birthday   = o.value("birthday").toString().toStdString();

        if (o.contains("phones") && o.value("phones").isObject()) {
            QJsonObject p = o.value("phones").toObject();
            c.numbers.number1 = p.value("work").toString().toStdString();
            c.numbers.number2 = p.value("home").toString().toStdString();
            c.numbers.number3 = p.value("office").toString().toStdString();
        }

        mainStorage[id] = c;
        maxId = std::max(maxId, id);

        if (!c.firstName.empty()) firstNameIndex[c.firstName] = id;
        if (!c.lastName.empty())  lastNameIndex[c.lastName] = id;
        if (!c.numbers.number1.empty()) phoneWorkIndex[c.numbers.number1] = id;
        if (!c.numbers.number2.empty()) phoneHomeIndex[c.numbers.number2] = id;
        if (!c.numbers.number3.empty()) phoneOfficeIndex[c.numbers.number3] = id;
        if (!c.email.empty()) emailIndex[c.email] = id;
    }

    index = std::max(index, maxId);
    if (root.contains("index")) {
        index = std::max(index, static_cast<unsigned int>(root.value("index").toInt(static_cast<int>(index))));
    }

    return true;
}

bool PhoneBook::add_contact(const Contact& contact, std::string* error)
{
    auto fail = [&](const std::string& msg) {
        if (error) *error = msg;
        return false;
    };

    // Required: first/last/email + at least one phone (Task requirements)
    if (!isValidName(contact.firstName)) return fail("Invalid first name.");
    if (!isValidName(contact.lastName))  return fail("Invalid last name.");
    if (!isValidEmail(contact.email))    return fail("Invalid email.");

    bool hasAtLeastOnePhone = false;

    auto checkPhone = [&](const std::string& p, const char* label) -> bool {
        if (p.empty()) return true;
        if (!isValidPhone(p)) {
            return fail(std::string("Invalid phone: ") + label);
        }
        hasAtLeastOnePhone = true;
        return true;
    };

    if (!checkPhone(contact.numbers.number1, "Work")) return false;
    if (!checkPhone(contact.numbers.number2, "Home")) return false;
    if (!checkPhone(contact.numbers.number3, "Office")) return false;

    if (!hasAtLeastOnePhone) return fail("At least one phone number is required.");

    // Optional fields validated if present
    if (!contact.middleName.empty() && !isValidName(contact.middleName))
        return fail("Invalid middle name.");

    if (!contact.birthday.empty() && !isValidBirthday(contact.birthday))
        return fail("Invalid birthday (must be dd-mm-yyyy and in the past).");

    // Store + indices
    const unsigned int newId = ++index;
    mainStorage[newId] = contact;

    firstNameIndex[contact.firstName] = newId;
    lastNameIndex[contact.lastName] = newId;

    if (!contact.numbers.number1.empty()) phoneWorkIndex[contact.numbers.number1] = newId;
    if (!contact.numbers.number2.empty()) phoneHomeIndex[contact.numbers.number2] = newId;
    if (!contact.numbers.number3.empty()) phoneOfficeIndex[contact.numbers.number3] = newId;

    emailIndex[contact.email] = newId;

    if (!save_to_file()) {
        // contact is still created; we just report persistence issue
        return fail("Contact created, but failed to save to file.");
    }

    return true;
}
bool PhoneBook::get_contact(unsigned int id, Contact* out) const
{
    auto it = mainStorage.find(id);
    if (it == mainStorage.end()) return false;
    if (out) *out = it->second;
    return true;
}

bool PhoneBook::remove_contact(unsigned int id, std::string* error)
{
    auto fail = [&](const std::string& msg) {
        if (error) *error = msg;
        return false;
    };

    auto it = mainStorage.find(id);
    if (it == mainStorage.end()) return fail("Contact not found.");

    const Contact& c = it->second;

    // Remove indices safely (only if they point to this ID)
    auto eraseIfMatches = [&](auto& mp, const std::string& key) {
        if (key.empty()) return;
        auto ix = mp.find(key);
        if (ix != mp.end() && ix->second == id) mp.erase(ix);
    };

    eraseIfMatches(firstNameIndex, c.firstName);
    eraseIfMatches(lastNameIndex,  c.lastName);
    eraseIfMatches(emailIndex,     c.email);

    eraseIfMatches(phoneWorkIndex,   c.numbers.number1);
    eraseIfMatches(phoneHomeIndex,   c.numbers.number2);
    eraseIfMatches(phoneOfficeIndex, c.numbers.number3);

    mainStorage.erase(it);

    if (!save_to_file()) {
        return fail("Contact deleted, but failed to save to file.");
    }

    return true;
}

bool PhoneBook::update_contact(unsigned int id, const Contact& updated, std::string* error)
{
    auto fail = [&](const std::string& msg) {
        if (error) *error = msg;
        return false;
    };

    auto it = mainStorage.find(id);
    if (it == mainStorage.end()) return fail("Contact not found.");

    // Validate required fields
    if (!isValidName(updated.firstName)) return fail("Invalid first name.");
    if (!isValidName(updated.lastName))  return fail("Invalid last name.");
    if (!isValidEmail(updated.email))    return fail("Invalid email.");

    bool hasAtLeastOnePhone = false;

    auto checkPhone = [&](const std::string& p, const char* label) -> bool {
        if (p.empty()) return true;
        if (!isValidPhone(p)) return fail(std::string("Invalid phone: ") + label);
        hasAtLeastOnePhone = true;
        return true;
    };

    if (!checkPhone(updated.numbers.number1, "Work")) return false;
    if (!checkPhone(updated.numbers.number2, "Home")) return false;
    if (!checkPhone(updated.numbers.number3, "Office")) return false;

    if (!hasAtLeastOnePhone) return fail("At least one phone number is required.");

    // Optional validation
    if (!updated.middleName.empty() && !isValidName(updated.middleName))
        return fail("Invalid middle name.");

    if (!updated.birthday.empty() && !isValidBirthday(updated.birthday))
        return fail("Invalid birthday (must be dd-mm-yyyy and in the past).");

    // Remove old indices (only if they point to this ID)
    const Contact& old = it->second;

    auto eraseIfMatches = [&](auto& mp, const std::string& key) {
        if (key.empty()) return;
        auto ix = mp.find(key);
        if (ix != mp.end() && ix->second == id) mp.erase(ix);
    };

    eraseIfMatches(firstNameIndex, old.firstName);
    eraseIfMatches(lastNameIndex,  old.lastName);
    eraseIfMatches(emailIndex,     old.email);

    eraseIfMatches(phoneWorkIndex,   old.numbers.number1);
    eraseIfMatches(phoneHomeIndex,   old.numbers.number2);
    eraseIfMatches(phoneOfficeIndex, old.numbers.number3);

    // Update stored contact
    it->second = updated;

    // Add new indices
    firstNameIndex[updated.firstName] = id;
    lastNameIndex[updated.lastName] = id;
    emailIndex[updated.email] = id;

    if (!updated.numbers.number1.empty()) phoneWorkIndex[updated.numbers.number1] = id;
    if (!updated.numbers.number2.empty()) phoneHomeIndex[updated.numbers.number2] = id;
    if (!updated.numbers.number3.empty()) phoneOfficeIndex[updated.numbers.number3] = id;

    if (!save_to_file()) {
        return fail("Contact updated, but failed to save to file.");
    }

    return true;
}
