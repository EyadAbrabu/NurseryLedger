#include "TestCheck.hpp"
#include "Ledger.hpp"

int main() {
    Ledger ledger;

    CHECK(ledger.registerChild(
        {101, "Ahmed", {2026, 11, 15}, 15000}
    ));

    CHECK(ledger.generateMonthlyFee(101, 2026, 11));
    CHECK(ledger.generateMonthlyFee(101, 2026, 12));

    // November: 150 euros due, 200 euros received.
    CHECK(ledger.recordPayment(
        {101, {2026, 11, 20}, 20000,
         PaymentMethod::Cash, 2026, 11}
    ));

    CHECK(ledger.getOutstandingBalance(101, 2026, 11) == 0);
    CHECK(ledger.getOutstandingBalance(101, 2026, 12) == 10000);
    CHECK(ledger.getAvailableCredit(101) == 0);

    // December: pay remaining 100 euros.
    CHECK(ledger.recordPayment(
        {101, {2026, 12, 20}, 10000,
         PaymentMethod::BankTransfer, 2026, 12}
    ));

    CHECK(ledger.getOutstandingBalance(101, 2026, 12) == 0);
    CHECK(ledger.getAvailableCredit(101) == 0);


    // TEST 2: Credit must not be used twice
{
    Ledger account;

    CHECK(account.registerChild(
        {202, "Sara", {2026, 11, 1}, 15000}
    ));

    CHECK(account.generateMonthlyFee(202, 2026, 11));
    CHECK(account.generateMonthlyFee(202, 2026, 12));
    CHECK(account.generateMonthlyFee(202, 2027, 1));

    // November: €150 fee, €350 payment = €200 excess.
    CHECK(account.recordPayment(
        {202, {2026, 11, 5}, 35000,
         PaymentMethod::Cash, 2026, 11}
    ));

    // €150 credit settles December; €50 settles January.
    CHECK(account.getOutstandingBalance(202, 2026, 12) == 0);
    CHECK(account.getOutstandingBalance(202, 2027, 1) == 10000);
    CHECK(account.getAvailableCredit(202) == 0);
}

// TEST 3: Later credit must not pay earlier debt
{
    Ledger account;

    CHECK(account.registerChild(
        {303, "Omar", {2026, 11, 15}, 15000}
    ));

    CHECK(account.generateMonthlyFee(303, 2026, 11));
    CHECK(account.generateMonthlyFee(303, 2026, 12));

    // Payment is explicitly assigned to December.
    CHECK(account.recordPayment(
        {303, {2026, 12, 20}, 20000,
         PaymentMethod::BankTransfer, 2026, 12}
    ));

    CHECK(account.getOutstandingBalance(303, 2026, 11) == 15000);
    CHECK(account.getOutstandingBalance(303, 2026, 12) == 0);
    CHECK(account.getAvailableCredit(303) == 5000);
}

// TEST 4: Generating fees out of order must not change balances
{
    Ledger account;

    CHECK(account.registerChild(
        {404, "Mariam", {2026, 11, 1}, 15000}
    ));

    CHECK(account.generateMonthlyFee(404, 2026, 12));
    CHECK(account.generateMonthlyFee(404, 2026, 11));

    CHECK(account.recordPayment(
        {404, {2026, 11, 10}, 20000,
         PaymentMethod::Cash, 2026, 11}
    ));

    CHECK(account.getOutstandingBalance(404, 2026, 12) == 10000);
}



    return 0;
}
