#ifndef FEE_OBLIGATION_HPP
#define FEE_OBLIGATION_HPP

#include <cstdint>
#include "Date.hpp"

struct FeeObligation {
    int childId;
    int billingYear;
    int billingMonth;
    Date dueDate;
    std::int64_t amountCents;
};

#endif

