#include "PhoneBook.h"
#include "Checkers.h"
#include <iostream>
#include <limits>
#include <regex>

void PhoneBook::contact_creation_menu()
{
    Contact contact;  // uses your default constructor

    // Clear any leftover newline from previous std::cin >>
  //  if (std::cin.peek() == '\n') {
 //       std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  //  }

    // ======================================================
    //           REQUIRED FIELDS (for creating contact)
    // ======================================================
    std::cout << "==============================\n";
    std::cout << "  REQUIRED FIELDS\n";
    std::cout << "  (must be filled to create a contact)\n";
    std::cout << "==============================\n";

    // First name (required)
    std::cout << "Enter FIRST name (required): ";
    std::getline(std::cin, contact.firstName);
    while (!isValidName(contact.firstName)) {
        std::cout << "Invalid first name.\n"
            "It must start with a letter, contain only letters, digits,\n"
            "spaces and hyphens, and cannot end with a hyphen.\n";
        std::cout << "Enter FIRST name (required): ";
        std::getline(std::cin, contact.firstName);
    }

    // Last name (required)
    std::cout << "Enter LAST name (required): ";
    std::getline(std::cin, contact.lastName);
    while (!isValidName(contact.lastName)) {
        std::cout << "Invalid last name. Try again.\n";
        std::cout << "Enter LAST name (required): ";
        std::getline(std::cin, contact.lastName);
    }

    // Email - AUTO-GENERATED with option to customize
std::string generatedEmail = generateEmail(contact.firstName, contact.lastName);
std::cout << "\nAuto-generated EMAIL: " << generatedEmail << "\n";
std::cout << "Press Enter to use this email, or type a custom email: ";

std::getline(std::cin, contact.email);

if (contact.email.empty()) {
    // User pressed Enter - use auto-generated email
    contact.email = generatedEmail;
    std::cout << "Using auto-generated email: " << contact.email << "\n";
} else {
    // User entered custom email - validate it
    std::cout << "Enter EMAIL (required, username@domain): ";
    std::getline(std::cin, contact.email);
    while (!isValidEmail(contact.email)) {
        std::cout << "Invalid email.\n"
            "Username and domain must contain only Latin letters and digits.\n";
        std::cout << "Enter EMAIL (required): ";
        std::getline(std::cin, contact.email);
    
        if (contact.email.empty()) {
            contact.email = generatedEmail;
            std::cout << "Using auto-generated email: " << contact.email << "\n";
            break;
        }
    }
}

    // Main phone (required) → stored as numbers.number1 (work)
    // ... after you've already asked for firstName, lastName, email ...

// ======================================================
//      PHONES: number1=work, number2=home, number3=office
//      (At least ONE of the three is required)
// ======================================================
    while (true) {
        // Work phone (numbers.number1)
        while (true) {
            std::cout << "Enter WORK phone (optional, leave empty to skip): ";
            std::getline(std::cin, contact.numbers.number1);

            if (contact.numbers.number1.empty() ||
                isValidPhone(contact.numbers.number1)) {
                break;
            }
            std::cout << "Invalid phone format. Must start with +7 or 8 and match allowed patterns.\n";
        }

        // Home phone (numbers.number2)
        while (true) {
            std::cout << "Enter HOME phone (optional, leave empty to skip): ";
            std::getline(std::cin, contact.numbers.number2);

            if (contact.numbers.number2.empty() ||
                isValidPhone(contact.numbers.number2)) {
                break;
            }
            std::cout << "Invalid phone format. Try again.\n";
        }

        // Office phone (numbers.number3)
        while (true) {
            std::cout << "Enter OFFICE phone (optional, leave empty to skip): ";
            std::getline(std::cin, contact.numbers.number3);

            if (contact.numbers.number3.empty() ||
                isValidPhone(contact.numbers.number3)) {
                break;
            }
            std::cout << "Invalid phone format. Try again.\n";
        }

        bool hasAtLeastOnePhone =
            !contact.numbers.number1.empty() ||
            !contact.numbers.number2.empty() ||
            !contact.numbers.number3.empty();

        if (!hasAtLeastOnePhone) {
            std::cout << "You must enter at least ONE phone number "
                "(work, home, or office).\n";
            std::cout << "Let's re-enter the phone numbers.\n\n";
        }
        else {
            break; // rule satisfied
        }
    }

    std::cout << "\nDo you want to fill ADDITIONAL FIELDS "
        "(middle name, address, birthday)? (y/n): ";

    // ======================================================
    //           ASK ABOUT ADDITIONAL FIELDS
    // ======================================================
    

    std::string answer;
    std::getline(std::cin, answer);
    char choice = (!answer.empty() ? answer[0] : 'n');

    if (choice == 'y' || choice == 'Y') {
        std::cout << "\n==============================\n";
        std::cout << "  ADDITIONAL FIELDS (optional)\n";
        std::cout << "==============================\n";

        // Middle name (optional)
        std::cout << "Enter MIDDLE name (optional, press Enter to skip): ";
        std::getline(std::cin, contact.middleName);
        while (!contact.middleName.empty() && !isValidName(contact.middleName)) {
            std::cout << "Invalid middle name. Try again "
                "(or press Enter to leave it empty): ";
            std::getline(std::cin, contact.middleName);
        }

        // Address (optional – at least one non-space character if provided)
        static const std::regex addressPattern(R"(^.*\S.*$)");

        std::cout << "Enter ADDRESS (optional, press Enter to skip): ";
        std::getline(std::cin, contact.address);
        while (!contact.address.empty() &&
            !std::regex_match(contact.address, addressPattern)) {
            std::cout << "Invalid address. Must contain at least one non-space character.\n";
            std::cout << "Enter ADDRESS (or press Enter to skip): ";
            std::getline(std::cin, contact.address);
        }

        // Birthday (optional, dd-mm-yyyy, must be valid if provided)
        std::cout << "Enter BIRTHDAY (optional, dd-mm-yyyy, press Enter to skip): ";
        std::getline(std::cin, contact.birthday);
        while (!contact.birthday.empty() && !isValidBirthday(contact.birthday)) {
            std::cout << "Invalid birthday.\n"
                "Use format dd-mm-yyyy, month 1–12, correct day for the month,\n"
                "leap years respected, and the date must be in the past.\n";
            std::cout << "Enter BIRTHDAY (or press Enter to skip): ";
            std::getline(std::cin, contact.birthday);
        }

    }
    else {
        // User chose to skip additional fields.
        // middleName, address, birthday, number2, number3 remain default ("").
        std::cout << "Skipping additional fields. Contact will be stored with defaults.\n";
    }

    // Finally, create and store contact
    create_contact(contact);
}

Contact PhoneBook::contact_search_menu()
{
   

    std::cout << "==============================\n";
    std::cout << "        SEARCH CONTACT\n";
    std::cout << "==============================\n";
    std::cout << "Choose search method:\n";
    std::cout << "  1) First name\n";
    std::cout << "  2) Last name\n";
    std::cout << "  3) Work phone\n";
    std::cout << "  4) Home phone\n";
    std::cout << "  5) Office phone\n";
    std::cout << "  6) Email\n";
    std::cout << "Enter choice (1-6): ";

    char method;
    std::cin >> method;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    while (method < '1' || method > '6') {
        std::cout << "Invalid choice. Enter 1-6: ";
        std::cin >> method;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    std::string value;

    switch (method) {
    case '1': // first name
        std::cout << "Enter FIRST name to search: ";
        std::getline(std::cin, value);
        while (!isValidName(value)) {
            std::cout << "Invalid name. Try again: ";
            std::getline(std::cin, value);
        }
        break;

    case '2': // last name
        std::cout << "Enter LAST name to search: ";
        std::getline(std::cin, value);
        while (!isValidName(value)) {
            std::cout << "Invalid name. Try again: ";
            std::getline(std::cin, value);
        }
        break;

    case '3': // work phone
        std::cout << "Enter WORK phone to search: ";
        std::getline(std::cin, value);
        while (!isValidPhone(value)) {
            std::cout << "Invalid phone format. Try again: ";
            std::getline(std::cin, value);
        }
        break;

    case '4': // home phone
        std::cout << "Enter HOME phone to search: ";
        std::getline(std::cin, value);
        while (!isValidPhone(value)) {
            std::cout << "Invalid phone format. Try again: ";
            std::getline(std::cin, value);
        }
        break;

    case '5': // office phone
        std::cout << "Enter OFFICE phone to search: ";
        std::getline(std::cin, value);
        while (!isValidPhone(value)) {
            std::cout << "Invalid phone format. Try again: ";
            std::getline(std::cin, value);
        }
        break;

    case '6': // email
        std::cout << "Enter EMAIL to search: ";
        std::getline(std::cin, value);
        while (!isValidEmail(value)) {
            std::cout << "Invalid email. Try again: ";
            std::getline(std::cin, value);
        }
        break;
    }

    // Call the actual search function with validated input
    Contact result = search(method, value);

    
    return result;
}

void PhoneBook::edit_contact()
{
    if (mainStorage.empty()) {
        std::cout << "Phone book is empty. Nothing to edit.\n";
        return;
    }

    // ---- MENU: choose how to find the contact ----
    std::cout << "==============================\n";
    std::cout << "        EDIT CONTACT\n";
    std::cout << "==============================\n";
    std::cout << "Choose search method to find the contact:\n";
    std::cout << "  1) First name\n";
    std::cout << "  2) Last name\n";
    std::cout << "  3) Work phone\n";
    std::cout << "  4) Home phone\n";
    std::cout << "  5) Office phone\n";
    std::cout << "  6) Email\n";
    std::cout << "Enter choice (1-6): ";

    char method;
    std::cin >> method;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    while (method < '1' || method > '6') {
        std::cout << "Invalid choice. Enter 1-6: ";
        std::cin >> method;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    // ---- Get search value and validate with the right checker ----
    std::string value;

    switch (method) {
    case '1': // first name
        std::cout << "Enter FIRST name to search: ";
        std::getline(std::cin, value);
        while (!isValidName(value)) {
            std::cout << "Invalid name. Try again: ";
            std::getline(std::cin, value);
        }
        break;

    case '2': // last name
        std::cout << "Enter LAST name to search: ";
        std::getline(std::cin, value);
        while (!isValidName(value)) {
            std::cout << "Invalid name. Try again: ";
            std::getline(std::cin, value);
        }
        break;

    case '3': // work phone
        std::cout << "Enter WORK phone to search: ";
        std::getline(std::cin, value);
        while (!isValidPhone(value)) {
            std::cout << "Invalid phone format. Try again: ";
            std::getline(std::cin, value);
        }
        break;

    case '4': // home phone
        std::cout << "Enter HOME phone to search: ";
        std::getline(std::cin, value);
        while (!isValidPhone(value)) {
            std::cout << "Invalid phone format. Try again: ";
            std::getline(std::cin, value);
        }
        break;

    case '5': // office phone
        std::cout << "Enter OFFICE phone to search: ";
        std::getline(std::cin, value);
        while (!isValidPhone(value)) {
            std::cout << "Invalid phone format. Try again: ";
            std::getline(std::cin, value);
        }
        break;

    case '6': // email
        std::cout << "Enter EMAIL to search: ";
        std::getline(std::cin, value);
        while (!isValidEmail(value)) {
            std::cout << "Invalid email. Try again: ";
            std::getline(std::cin, value);
        }
        break;
    }

    // ---- Find the contact ID using the right index map ----
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
        return;
    }

    auto itMain = mainStorage.find(id);
    if (itMain == mainStorage.end()) {
        std::cout << "Internal error: contact not found in main storage.\n";
        return;
    }

    std::cout << "\nContact found (ID = " << id << "):\n";
    itMain->second.print_contact();  // uses your Contact::print_contact()

    // ---- Call implementation helper to edit fields and update maps ----
    edit_contact_fields(*this, id);
}
void PhoneBook::delete_contact()
{
    if (mainStorage.empty()) {
        std::cout << "Phone book is empty. Nothing to delete.\n";
        return;
    }

    // ---- MENU: choose how to find the contact ----
    std::cout << "==============================\n";
    std::cout << "        DELETE CONTACT\n";
    std::cout << "==============================\n";
    std::cout << "Choose search method to find the contact:\n";
    std::cout << "  1) First name\n";
    std::cout << "  2) Last name\n";
    std::cout << "  3) Work phone\n";
    std::cout << "  4) Home phone\n";
    std::cout << "  5) Office phone\n";
    std::cout << "  6) Email\n";
    std::cout << "Enter choice (1-6): ";

    char method;
    std::cin >> method;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    while (method < '1' || method > '6') {
        std::cout << "Invalid choice. Enter 1-6: ";
        std::cin >> method;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    // ---- Get search value and validate with the right checker ----
    std::string value;

    switch (method) {
    case '1': // first name
        std::cout << "Enter FIRST name to search: ";
        std::getline(std::cin, value);
        while (!isValidName(value)) {
            std::cout << "Invalid name. Try again: ";
            std::getline(std::cin, value);
        }
        break;

    case '2': // last name
        std::cout << "Enter LAST name to search: ";
        std::getline(std::cin, value);
        while (!isValidName(value)) {
            std::cout << "Invalid name. Try again: ";
            std::getline(std::cin, value);
        }
        break;

    case '3': // work phone
        std::cout << "Enter WORK phone to search: ";
        std::getline(std::cin, value);
        while (!isValidPhone(value)) {
            std::cout << "Invalid phone format. Try again: ";
            std::getline(std::cin, value);
        }
        break;

    case '4': // home phone
        std::cout << "Enter HOME phone to search: ";
        std::getline(std::cin, value);
        while (!isValidPhone(value)) {
            std::cout << "Invalid phone format. Try again: ";
            std::getline(std::cin, value);
        }
        break;

    case '5': // office phone
        std::cout << "Enter OFFICE phone to search: ";
        std::getline(std::cin, value);
        while (!isValidPhone(value)) {
            std::cout << "Invalid phone format. Try again: ";
            std::getline(std::cin, value);
        }
        break;

    case '6': // email
        std::cout << "Enter EMAIL to search: ";
        std::getline(std::cin, value);
        while (!isValidEmail(value)) {
            std::cout << "Invalid email. Try again: ";
            std::getline(std::cin, value);
        }
        break;
    }

    // ---- Find the contact ID using the right index map ----
    unsigned int id = 0;
    bool found = false;

    switch (method) {
    case '1': { // first name
        auto it = firstNameIndex.find(value);
        if (it != firstNameIndex.end()) { id = it->second; found = true; }
        break;
    }
    case '2': { // last name
        auto it = lastNameIndex.find(value);
        if (it != lastNameIndex.end()) { id = it->second; found = true; }
        break;
    }
    case '3': { // work phone
        auto it = phoneWorkIndex.find(value);
        if (it != phoneWorkIndex.end()) { id = it->second; found = true; }
        break;
    }
    case '4': { // home phone
        auto it = phoneHomeIndex.find(value);
        if (it != phoneHomeIndex.end()) { id = it->second; found = true; }
        break;
    }
    case '5': { // office phone
        auto it = phoneOfficeIndex.find(value);
        if (it != phoneOfficeIndex.end()) { id = it->second; found = true; }
        break;
    }
    case '6': { // email
        auto it = emailIndex.find(value);
        if (it != emailIndex.end()) { id = it->second; found = true; }
        break;
    }
    }

    if (!found) {
        std::cout << "No contact found for the given search value.\n";
        return;
    }

    auto itMain = mainStorage.find(id);
    if (itMain == mainStorage.end()) {
        std::cout << "Internal error: contact not found in main storage.\n";
        return;
    }

    // ---- Show contact and confirm ----
    Contact& contact = itMain->second;
    
    contact.print_contact();

    std::cout << "\nAre you sure you want to DELETE this contact? (y/n): ";
    std::string answer;
    std::getline(std::cin, answer);
    char confirm = answer.empty() ? 'n' : answer[0];

    if (confirm != 'y' && confirm != 'Y') {
        std::cout << "Deletion cancelled.\n";
        return;
    }

    // ---- Call implementation helper ----
    delete_contact_impl(*this, id);
}
void PhoneBook::contact_sort_menu()
{
    if (mainStorage.empty()) {
        std::cout << "Phone book is empty. Nothing to sort.\n";
        return;
    }

    std::cout << "==============================\n";
    std::cout << "        SORT CONTACTS\n";
    std::cout << "==============================\n";
    std::cout << "Choose sort method:\n";
    std::cout << "  1) By FIRST name (ascending)\n";
    std::cout << "  2) By LAST name  (ascending)\n";
    std::cout << "Enter choice (1-2): ";

    char method;
    std::cin >> method;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    while (method != '1' && method != '2') {
        std::cout << "Invalid choice. Enter 1 or 2: ";
        std::cin >> method;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    // Call the actual sort+list function
    list_sorted_contacts(method);
}
