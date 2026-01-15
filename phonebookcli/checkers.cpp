#include "Checkers.h"

#include <regex>
#include <algorithm>
#include <array>
#include <cctype>
#include <ctime>

// ---------- ONE COMMON TRIMMER ----------
// removes leading and trailing whitespace only
static std::string trim(const std::string& s) {
    std::size_t start = 0;
    while (start < s.size() &&
        std::isspace(static_cast<unsigned char>(s[start]))) {
        ++start;
    }
    if (start == s.size()) {
        return "";
    }

    std::size_t end = s.size() - 1;
    while (end > start &&
        std::isspace(static_cast<unsigned char>(s[end]))) {
        --end;
    }

    return s.substr(start, end - start + 1);
}

// ---------- DATE HELPERS ----------

static bool isLeapYear(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

static int daysInMonth(int month, int year) {
    if (month < 1 || month > 12) return 0;

    constexpr std::array<int, 12> days = {
        31, 28, 31, 30, 31, 30,
        31, 31, 30, 31, 30, 31
    };

    if (month == 2 && isLeapYear(year)) {
        return 29;
    }
    return days[month - 1];
}

// ---------- NAME CHECKER ----------
// Rules:
// - must start with a LETTER
// - can contain only letters, digits, spaces, and hyphens
// - cannot end with a hyphen ('-')
bool isValidName(const std::string& rawName) {
    std::string name = trim(rawName);
    if (name.empty()) {
        return false;
    }

    static const std::regex pattern(R"(^[A-Za-z][A-Za-z0-9 -]*$)");

    if (!std::regex_match(name, pattern)) {
        return false;
    }

    if (name.back() == '-') {
        return false;
    }

    return true;
}

// ---------- PHONE CHECKER ----------
// Rules:
// - must start with +7 or 8
// - allowed formats (area code 3 digits):
//   +7XXXXXXXXXX
//   8XXXXXXXXXX
//   +7(XXX)XXXXXXX
//   8(XXX)XXXXXXX
//   +7(XXX)XXX-XX-XX
//   8(XXX)XXX-XX-XX
bool isValidPhone(const std::string& rawPhone) {
    std::string phone = trim(rawPhone);
    if (phone.empty()) {
        return false;
    }

    static const std::regex pattern(
        R"(^(?:\+7|8)(?:\d{10}|\(\d{3}\)\d{7}|\(\d{3}\)\d{3}-\d{2}-\d{2})$)"
    );

    return std::regex_match(phone, pattern);
}

// ---------- BIRTHDAY CHECKER ----------
// Format: dd-mm-yyyy
// - valid day/month/year (with leap years)
// - must be strictly less than today's date
bool isValidBirthday(const std::string& rawDate) {
    std::string date = trim(rawDate);
    if (date.empty()) return false;

    static const std::regex pattern(R"(^(\d{2})-(\d{2})-(\d{4})$)");
    std::smatch match;

    if (!std::regex_match(date, match, pattern)) {
        return false;
    }

    int day = std::stoi(match[1].str());
    int month = std::stoi(match[2].str());
    int year = std::stoi(match[3].str());

    if (month < 1 || month > 12) return false;

    int maxDay = daysInMonth(month, year);
    if (maxDay == 0) return false;
    if (day < 1 || day > maxDay) return false;

    // current date
    std::time_t t = std::time(nullptr);
    std::tm* now = std::localtime(&t);

    int curYear = now->tm_year + 1900;
    int curMonth = now->tm_mon + 1;
    int curDay = now->tm_mday;

    // must be strictly in the past
    if (year > curYear) return false;
    if (year == curYear && month > curMonth) return false;
    if (year == curYear && month == curMonth && day >= curDay) return false;

    return true;
}

// ---------- EMAIL CHECKER ----------
// Rules:
//  - username: Latin letters and digits
//  - exactly one '@' separating username and domain
//  - domain: Latin letters and digits
//  - all spaces (including around '@') are ignored
bool isValidEmail(const std::string& rawEmail) {
    // 1) trim outer spaces once with our common trimmer
    std::string email = trim(rawEmail);

    // 2) remove any remaining whitespace inside (before/after '@', etc.)
    email.erase(
        std::remove_if(
            email.begin(), email.end(),
            [](unsigned char ch) { return std::isspace(ch); }
        ),
        email.end()
    );

    if (email.empty()) {
        return false;
    }

    // Note: dot in domain must be a literal '.' (escaped).
    static const std::regex pattern(R"(^[A-Za-z0-9]+@[A-Za-z0-9]+$)");

    return std::regex_match(email, pattern);
}
// ---------- EMAIL GENERATOR ----------
// Generates email in format: lastname.firstletter@domain.com
// Example: "John Doe" -> "doe.j@phonebook.com"
std::string generateEmail(const std::string& firstName, const std::string& lastName) {
    if (firstName.empty() || lastName.empty()) {
        return "";
    }
    
    // Convert to lowercase for consistency
    std::string lowerLastName = lastName;
    std::string firstLetter(1, firstName[0]);
    
    // Convert to lowercase
    std::transform(lowerLastName.begin(), lowerLastName.end(), 
                   lowerLastName.begin(), ::tolower);
    std::transform(firstLetter.begin(), firstLetter.end(), 
                   firstLetter.begin(), ::tolower);
    
    // Remove spaces and hyphens from last name
    lowerLastName.erase(
        std::remove_if(lowerLastName.begin(), lowerLastName.end(),
            [](char c) { return c == ' ' || c == '-'; }),
        lowerLastName.end()
    );
    
    return lowerLastName + firstLetter + "@gmail";
}