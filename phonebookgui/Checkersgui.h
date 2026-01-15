#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <string>

bool isValidName(const std::string& rawName);
bool isValidPhone(const std::string& rawPhone);
bool isValidBirthday(const std::string& rawDate);   // dd-mm-yyyy
bool isValidEmail(const std::string& rawEmail);
std::string generateEmail(const std::string& firstName, const std::string& lastName);
