#include "TestCheck.hpp"
#include "Ledger.hpp"

int main() {
    Ledger ledger;

    // Arrange: register a child and generate a fee
    Child ahmed{101, "Ahmed", {2026, 11, 15}, 15000};

    CHECK(ledger.registerChild(ahmed));
    CHECK(ledger.generateMonthlyFee(101, 2026, 11));

    // Positive test: valid payment of 100 euros
    Payment validPayment{
        101,
        {2026, 11, 20},
        10000,
        PaymentMethod::Cash,
        2026,
        11
    };

    CHECK(ledger.recordPayment(validPayment));

    // Negative test: payment with a negative amount
    Payment invalidPayment{
        101,
        {2026, 11, 20},
        -5000,
        PaymentMethod::Cash,
        2026,
        11
    };

    CHECK(!ledger.recordPayment(invalidPayment));

    // Verify only the valid payment was stored
    const auto& payments = ledger.getPayments();

    CHECK(payments.size() == 1);
    CHECK(payments[0].amountCents == 10000);
    CHECK(payments[0].childId == 101);


    // Partial payment: 150 euros due, 100 euros paid
CHECK(ledger.getOutstandingBalance(101, 2026, 11) == 5000);

// Second valid payment completes the obligation
Payment remainingPayment{
    101,
    {2026, 11, 25},
    5000,
    PaymentMethod::BankTransfer,
    2026,
    11
};

CHECK(ledger.recordPayment(remainingPayment));
CHECK(ledger.getOutstandingBalance(101, 2026, 11) == 0);



// Reject unsupported payment methods.
{
    Ledger account;

    CHECK(account.registerChild(
        {303, "Test Child", {2026, 11, 1}, 15000}
    ));

    CHECK(account.generateMonthlyFee(303, 2026, 11));

    const auto before = account.getPayments().size();

    CHECK(!account.recordPayment(
        {303, {2026, 11, 5}, 15000,
         static_cast<PaymentMethod>(99), 2026, 11}
    ));

    CHECK(account.getPayments().size() == before);
}





    return 0;
}
