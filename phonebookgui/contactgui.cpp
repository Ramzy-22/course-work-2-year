#include "Contactgui.h"
#include <iostream>
//Phone
Phone::Phone(std::string number1, std::string number2, std::string number3)
{
	this->number1 = number1;
	this->number2 = number2;
	this->number3 = number3;
}
Phone::Phone(const Phone& phone)
{
	this->number1 = phone.number1;
	this->number2 = phone.number2;
	this->number3 = phone.number3;
}
Phone::~Phone(){}
void Phone::print_number() const{
	std::cout << "Work: " << number1 << std::endl << "Home: " << number2 << std::endl << "office: " << number3 << std::endl;
}
//Contact
Contact::Contact(std::string firstName, std::string middleName, std::string lastName,
	Phone numbers, std::string email,std::string address , std::string birthday) :
firstName(firstName),middleName(middleName),lastName(lastName), numbers(numbers),
email(email),address(address), birthday(birthday)
{
	
}

Contact::Contact(const Contact& contact) {
	this->firstName = contact.firstName;
	this->middleName = contact.middleName;
	this->lastName = contact.lastName;
	this->numbers = contact.numbers;
	this->email = contact.email;
	this->address = contact.address;
	this->birthday = contact.birthday;
}
Contact::~Contact() {}
void Contact::set_contact(std::string firstName, std::string middleName, std::string lastName,
	Phone numbers, std::string email,std::string address , std::string birthday)
{
	this->firstName = firstName;
	this->middleName = middleName;
	this->lastName = lastName;
	this->numbers = numbers;
	this->email = email;
	this->address = address;
	this->birthday = birthday;
}
Contact Contact::get_contact() {
	return *this;
}
void Contact::print_contact() const{
		std::cout << "First name: " << firstName << std::endl
				  << "Middle Name: " << middleName << std::endl
				  << "Last Name: " << lastName << std::endl;
					numbers.print_number();
		std::cout << "Email: " << email << std::endl
				  << "Address: " << address << std::endl
				  <<"Birthday: " << birthday << std::endl;
}
 
