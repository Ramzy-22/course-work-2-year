#pragma once
#include <string>

struct Phone {
	std::string number1;
	std::string number2;
	std::string number3;
	Phone(std::string number1 = "", std::string number2 = "", std::string number3 = "");
	Phone(const Phone& phone);
	void print_number()const;
	~Phone();
};

struct Contact {
public:
	std::string firstName;
	std::string middleName;
	std::string lastName;
	Phone numbers;
	std::string email;
	std::string address;
	std::string birthday;
public:
	Contact(std::string firstName ="", std::string middleName="", std::string lastName="",
		 Phone numbers= {"","",""}, std::string email = "", std::string address = "", std::string birthday = "");
	Contact(const Contact& contact);
	~Contact();
	void set_contact(std::string firstName, std::string middleName, std::string lastName,
		Phone numbers, std::string email,std::string address , std::string birthday);
	Contact get_contact();
	void print_contact()const;

};