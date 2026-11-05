#include <cassert>
#include "Ledger.hpp"

int main() {
    Ledger ledger;

    assert(ledger.registerChild(
        {101, "Ahmed", {2026, 11, 15}, 15000}
    ));

    assert(ledger.generateMonthlyFee(101, 2026, 11));

    assert(ledger.recordPayment(
        {101, {2026, 11, 20}, 20000,
         PaymentMethod::Cash, 2026, 11}
    ));

    assert(ledger.getAvailableCredit(101) == 5000);

    // Refund €30 of the €50 unused credit.
    assert(ledger.recordRefund(
        {101, {2026, 11, 21}, 3000,
         PaymentMethod::Cash, 2026, 11}
    ));

    assert(ledger.getAvailableCredit(101) == 2000);
    assert(ledger.getPayments().size() == 1);
    assert(ledger.getRefunds().size() == 1);

    // Cannot refund more than the remaining credit.
    assert(!ledger.recordRefund(
        {101, {2026, 11, 22}, 3000,
         PaymentMethod::Cash, 2026, 11}
    ));

    // December fee is reduced only by remaining credit.
    assert(ledger.generateMonthlyFee(101, 2026, 12));
    assert(ledger.getOutstandingBalance(101, 2026, 12) == 13000);

    // Credit already applied to December cannot be refunded.
    assert(!ledger.recordRefund(
        {101, {2026, 12, 1}, 1000,
         PaymentMethod::Cash, 2026, 11}
    ));


    
// TEST 4: Invalid refund data must be rejected.
assert(!ledger.recordRefund(
    {101, {2026, 2, 30}, 100,
     PaymentMethod::Cash, 2026, 11}
));

assert(!ledger.recordRefund(
    {101, {2026, 12, 3}, -100,
     PaymentMethod::Cash, 2026, 11}
));

assert(!ledger.recordRefund(
    {999, {2026, 12, 3}, 100,
     PaymentMethod::Cash, 2026, 11}
));

assert(ledger.getRefunds().size() == 1);

// TEST 5: A second child's credit is independent.
{
    Ledger other;

    assert(other.registerChild(
        {202, "Sara", {2026, 11, 1}, 15000}
    ));

    assert(other.generateMonthlyFee(202, 2026, 11));

    assert(other.recordPayment(
        {202, {2026, 11, 5}, 18000,
         PaymentMethod::Cash, 2026, 11}
    ));

    assert(other.getAvailableCredit(202) == 3000);
    assert(other.getAvailableCredit(101) == 0);

    assert(!other.recordRefund(
        {101, {2026, 11, 6}, 1000,
         PaymentMethod::Cash, 2026, 11}
    ));

    assert(other.getAvailableCredit(202) == 3000);
}


// TEST 6: Multiple partial refunds of unused credit.
{
    Ledger account;

    assert(account.registerChild(
        {505, "Lina", {2026, 11, 1}, 15000}
    ));

    assert(account.generateMonthlyFee(505, 2026, 11));

    assert(account.recordPayment(
        {505, {2026, 11, 5}, 20000,
         PaymentMethod::Cash, 2026, 11}
    ));

    // €50 credit exists.
    assert(account.getAvailableCredit(505) == 5000);

    // First refund: €30.
    assert(account.recordRefund(
        {505, {2026, 11, 6}, 3000,
         PaymentMethod::Cash, 2026, 11}
    ));

    assert(account.getAvailableCredit(505) == 2000);

    // Second refund: remaining €20.
    assert(account.recordRefund(
        {505, {2026, 11, 7}, 2000,
         PaymentMethod::BankTransfer, 2026, 11}
    ));

    assert(account.getAvailableCredit(505) == 0);
    assert(account.getRefunds().size() == 2);

    // A third refund must be rejected.
    assert(!account.recordRefund(
        {505, {2026, 11, 8}, 100,
         PaymentMethod::Cash, 2026, 11}
    ));

    assert(account.getRefunds().size() == 2);
    assert(account.getPayments().size() == 1);

    // No credit remains to reduce December's fee.
    assert(account.generateMonthlyFee(505, 2026, 12));
    assert(account.getOutstandingBalance(505, 2026, 12) == 15000);
}




// TEST 7: Refunds must not affect another child's credit.
{
    Ledger account;

    assert(account.registerChild(
        {601, "Omar", {2026, 11, 1}, 15000}
    ));

    assert(account.registerChild(
        {602, "Sara", {2026, 11, 1}, 15000}
    ));

    assert(account.generateMonthlyFee(601, 2026, 11));
    assert(account.generateMonthlyFee(602, 2026, 11));

    assert(account.recordPayment(
        {601, {2026, 11, 5}, 20000,
         PaymentMethod::Cash, 2026, 11}
    ));

    assert(account.recordPayment(
        {602, {2026, 11, 5}, 18000,
         PaymentMethod::Cash, 2026, 11}
    ));

    assert(account.getAvailableCredit(601) == 5000);
    assert(account.getAvailableCredit(602) == 3000);

    // Refund Omar's €50 credit.
    assert(account.recordRefund(
        {601, {2026, 11, 6}, 5000,
         PaymentMethod::Cash, 2026, 11}
    ));

    assert(account.getAvailableCredit(601) == 0);

    // Sara's €30 credit must remain untouched.
    assert(account.getAvailableCredit(602) == 3000);

    // Omar cannot refund Sara's money.
    assert(!account.recordRefund(
        {601, {2026, 11, 7}, 1000,
         PaymentMethod::Cash, 2026, 11}
    ));

    assert(account.getAvailableCredit(602) == 3000);
    assert(account.getRefunds().size() == 1);
}






    return 0;
}

