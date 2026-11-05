#ifndef PAYMENT_HPP
#define PAYMENT_HPP

#include <cstdint>
#include "Date.hpp"

enum class PaymentMethod {
    Cash,
    BankTransfer
};

struct Payment {
    int childId;
    Date receivedDate;
    std::int64_t amountCents;
    PaymentMethod method;
    int billingYear;
    int billingMonth;
};

#endif

