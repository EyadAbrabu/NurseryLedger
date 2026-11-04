#include "Date.hpp"

bool isLeapYear(int year) {
    return (year % 400 == 0) ||
           (year % 4 == 0 && year % 100 != 0);
}

bool Date::isValid() const {
    if (year < 1 || month < 1 || month > 12) {
        return false;
    }

    int daysInMonth = 31;

    if (month == 4 || month == 6 ||
        month == 9 || month == 11) {
        daysInMonth = 30;
    }
    else if (month == 2) {
        daysInMonth = isLeapYear(year) ? 29 : 28;
    }

    return day >= 1 && day <= daysInMonth;
}
