#ifndef DATE_HPP
#define DATE_HPP

struct Date {
    int year;
    int month;
    int day;

    bool isValid() const;
};

#endif
