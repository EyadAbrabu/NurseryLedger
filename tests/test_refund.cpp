#include "TestCheck.hpp"
#include "Ledger.hpp"

int main() {
    Ledger ledger;

    CHECK(ledger.registerChild(
        {101, "Ahmed", {2026, 11, 15}, 15000}
    ));

    CHECK(ledger.generateMonthlyFee(101, 2026, 11));

    CHECK(ledger.recordPayment(
        {101, {2026, 11, 20}, 20000,
         PaymentMethod::Cash, 2026, 11}
    ));

    CHECK(ledger.getAvailableCredit(101) == 5000);

    // Refund €30 of the €50 unused credit.
    CHECK(ledger.recordRefund(
        {101, {2026, 11, 21}, 3000,
         PaymentMethod::Cash, 2026, 11}
    ));

    CHECK(ledger.getAvailableCredit(101) == 2000);
    CHECK(ledger.getPayments().size() == 1);
    CHECK(ledger.getRefunds().size() == 1);

    // Cannot refund more than the remaining credit.
    CHECK(!ledger.recordRefund(
        {101, {2026, 11, 22}, 3000,
         PaymentMethod::Cash, 2026, 11}
    ));

    // December fee is reduced only by remaining credit.
    CHECK(ledger.generateMonthlyFee(101, 2026, 12));
    CHECK(ledger.getOutstandingBalance(101, 2026, 12) == 13000);

    // Credit already applied to December cannot be refunded.
    CHECK(!ledger.recordRefund(
        {101, {2026, 12, 1}, 1000,
         PaymentMethod::Cash, 2026, 11}
    ));


    
// TEST 4: Invalid refund data must be rejected.
CHECK(!ledger.recordRefund(
    {101, {2026, 2, 30}, 100,
     PaymentMethod::Cash, 2026, 11}
));

CHECK(!ledger.recordRefund(
    {101, {2026, 12, 3}, -100,
     PaymentMethod::Cash, 2026, 11}
));

CHECK(!ledger.recordRefund(
    {999, {2026, 12, 3}, 100,
     PaymentMethod::Cash, 2026, 11}
));

CHECK(ledger.getRefunds().size() == 1);

// TEST 5: A second child's credit is independent.
{
    Ledger other;

    CHECK(other.registerChild(
        {202, "Sara", {2026, 11, 1}, 15000}
    ));

    CHECK(other.generateMonthlyFee(202, 2026, 11));

    CHECK(other.recordPayment(
        {202, {2026, 11, 5}, 18000,
         PaymentMethod::Cash, 2026, 11}
    ));

    CHECK(other.getAvailableCredit(202) == 3000);
    CHECK(other.getAvailableCredit(101) == 0);

    CHECK(!other.recordRefund(
        {101, {2026, 11, 6}, 1000,
         PaymentMethod::Cash, 2026, 11}
    ));

    CHECK(other.getAvailableCredit(202) == 3000);
}


// TEST 6: Multiple partial refunds of unused credit.
{
    Ledger account;

    CHECK(account.registerChild(
        {505, "Lina", {2026, 11, 1}, 15000}
    ));

    CHECK(account.generateMonthlyFee(505, 2026, 11));

    CHECK(account.recordPayment(
        {505, {2026, 11, 5}, 20000,
         PaymentMethod::Cash, 2026, 11}
    ));

    // €50 credit exists.
    CHECK(account.getAvailableCredit(505) == 5000);

    // First refund: €30.
    CHECK(account.recordRefund(
        {505, {2026, 11, 6}, 3000,
         PaymentMethod::Cash, 2026, 11}
    ));

    CHECK(account.getAvailableCredit(505) == 2000);

    // Second refund: remaining €20.
    CHECK(account.recordRefund(
        {505, {2026, 11, 7}, 2000,
         PaymentMethod::BankTransfer, 2026, 11}
    ));

    CHECK(account.getAvailableCredit(505) == 0);
    CHECK(account.getRefunds().size() == 2);

    // A third refund must be rejected.
    CHECK(!account.recordRefund(
        {505, {2026, 11, 8}, 100,
         PaymentMethod::Cash, 2026, 11}
    ));

    CHECK(account.getRefunds().size() == 2);
    CHECK(account.getPayments().size() == 1);

    // No credit remains to reduce December's fee.
    CHECK(account.generateMonthlyFee(505, 2026, 12));
    CHECK(account.getOutstandingBalance(505, 2026, 12) == 15000);
}




// TEST 7: Refunds must not affect another child's credit.
{
    Ledger account;

    CHECK(account.registerChild(
        {601, "Omar", {2026, 11, 1}, 15000}
    ));

    CHECK(account.registerChild(
        {602, "Sara", {2026, 11, 1}, 15000}
    ));

    CHECK(account.generateMonthlyFee(601, 2026, 11));
    CHECK(account.generateMonthlyFee(602, 2026, 11));

    CHECK(account.recordPayment(
        {601, {2026, 11, 5}, 20000,
         PaymentMethod::Cash, 2026, 11}
    ));

    CHECK(account.recordPayment(
        {602, {2026, 11, 5}, 18000,
         PaymentMethod::Cash, 2026, 11}
    ));

    CHECK(account.getAvailableCredit(601) == 5000);
    CHECK(account.getAvailableCredit(602) == 3000);

    // Refund Omar's €50 credit.
    CHECK(account.recordRefund(
        {601, {2026, 11, 6}, 5000,
         PaymentMethod::Cash, 2026, 11}
    ));

    CHECK(account.getAvailableCredit(601) == 0);

    // Sara's €30 credit must remain untouched.
    CHECK(account.getAvailableCredit(602) == 3000);

    // Omar cannot refund Sara's money.
    CHECK(!account.recordRefund(
        {601, {2026, 11, 7}, 1000,
         PaymentMethod::Cash, 2026, 11}
    ));

    CHECK(account.getAvailableCredit(602) == 3000);
    CHECK(account.getRefunds().size() == 1);
}






    return 0;
}

