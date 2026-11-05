#include <cassert>
#include "Ledger.hpp"

int main() {
    Ledger ledger;

    // Arrange: register a child and generate a fee
    Child ahmed{101, "Ahmed", {2026, 11, 15}, 15000};

    assert(ledger.registerChild(ahmed));
    assert(ledger.generateMonthlyFee(101, 2026, 11));

    // Positive test: valid payment of 100 euros
    Payment validPayment{
        101,
        {2026, 11, 20},
        10000,
        PaymentMethod::Cash,
        2026,
        11
    };

    assert(ledger.recordPayment(validPayment));

    // Negative test: payment with a negative amount
    Payment invalidPayment{
        101,
        {2026, 11, 20},
        -5000,
        PaymentMethod::Cash,
        2026,
        11
    };

    assert(!ledger.recordPayment(invalidPayment));

    // Verify only the valid payment was stored
    const auto& payments = ledger.getPayments();

    assert(payments.size() == 1);
    assert(payments[0].amountCents == 10000);
    assert(payments[0].childId == 101);


    // Partial payment: 150 euros due, 100 euros paid
assert(ledger.getOutstandingBalance(101, 2026, 11) == 5000);

// Second valid payment completes the obligation
Payment remainingPayment{
    101,
    {2026, 11, 25},
    5000,
    PaymentMethod::BankTransfer,
    2026,
    11
};

assert(ledger.recordPayment(remainingPayment));
assert(ledger.getOutstandingBalance(101, 2026, 11) == 0);


    return 0;
}
