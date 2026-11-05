#ifndef REFUND_HPP
#define REFUND_HPP

#include <cstdint>
#include "Date.hpp"
#include "Payment.hpp"

struct Refund {
    int childId;
    Date issuedDate;
    std::int64_t amountCents;
    PaymentMethod method;
    int billingYear;
    int billingMonth;
};

#endif
