#include <iostream>
#include <string>
#include "PhoneBook.h"
//this is the command line user interfaced 
int main()
{
    PhoneBook phoneBook;
    std::string command;

    std::cout << "=====================================\n";
    std::cout << "        PHONE BOOK APPLICATION\n";
    std::cout << "=====================================\n";
    std::cout << "Type the menu number to perform an action.\n";
    std::cout << "Type 'quit' to exit the application.\n\n";

    while (true) {
        std::cout << "--------------- MAIN MENU ---------------\n";
        std::cout << "1) Create contact\n";
        std::cout << "2) Search contact\n";
        std::cout << "3) Edit contact\n";
        std::cout << "4) Delete contact\n";
        std::cout << "5) List contacts (sorted)\n";
        std::cout << "-----------------------------------------\n";
        std::cout << "Enter choice (1-5 or 'quit'): ";

        if (!std::getline(std::cin, command)) {
            std::cout << "\nInput stream closed. Exiting.\n";
            break;
        }

        // Handle quit
        if (command == "quit" || command == "QUIT" || command == "Quit") {
            std::cout << "Exiting phone book. Goodbye!\n";
            break;
        }

        // Ignore empty lines
        if (command.empty()) {
            std::cout << "Unknown command. Please enter 1-5 or 'quit'.\n\n";
            continue;
        }

        // Take only the first character (this avoids the '\r' problem on Windows)
        char choice = command[0];

        switch (choice) {
        case '1':
            // CREATE CONTACT
            phoneBook.contact_creation_menu();
            break;

        case '2': {
            // SEARCH CONTACT
            Contact result = phoneBook.contact_search_menu();
            if (!result.firstName.empty() ||
                !result.lastName.empty() ||
                !result.email.empty()) {
                std::cout << "\nContact found:\n";
                result.print_contact();
            }
            break;
        }

        case '3':
            // EDIT CONTACT
            phoneBook.edit_contact();
            break;

        case '4':
            // DELETE CONTACT
            phoneBook.delete_contact();
            break;

        case '5':
            // LIST SORTED CONTACTS
            phoneBook.contact_sort_menu();
            break;

        default:
            std::cout << "Unknown command. Please enter 1-5 or 'quit'.\n";
            break;
        }

        std::cout << "\n";

        
        

    }

    return 0;
}
