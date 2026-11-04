#include <iostream>
#include "Date.hpp"

int main() {
    Date date1{2026, 11, 15};
    Date date2{2026, 2, 31};
    Date date3{2028, 2, 29};
    Date date4{1900, 2, 29};
    Date date5{2000, 2, 29};

    std::cout << date1.isValid() << '\n';
    std::cout << date2.isValid() << '\n';
    std::cout << date3.isValid() << '\n';
    std::cout << date4.isValid() << '\n';
    std::cout << date5.isValid() << '\n';

    return 0;
}
