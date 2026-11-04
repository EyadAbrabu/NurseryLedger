#ifndef CHILD_HPP
#define CHILD_HPP

#include <cstdint>
#include <string>
#include "Date.hpp"

struct Child {
    int id;
    std::string name;
    Date enrollmentDate;
    std::int64_t monthlyFeeCents;
};

#endif
