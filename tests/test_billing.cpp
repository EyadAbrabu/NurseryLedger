#include <cassert>
#include "Ledger.hpp"

int main() {
    Ledger ledger;

    Child ahmed{101, "Ahmed", {2026, 11, 15}, 15000};
    assert(ledger.registerChild(ahmed));

    // First monthly fee must succeed
    assert(ledger.generateMonthlyFee(101, 2026, 11));

    // Duplicate monthly fee must fail
    assert(!ledger.generateMonthlyFee(101, 2026, 11));

    // Next month's fee must succeed
    assert(ledger.generateMonthlyFee(101, 2026, 12));

    // Unknown child must fail
    assert(!ledger.generateMonthlyFee(999, 2026, 11));

    // Billing before enrollment must fail
    assert(!ledger.generateMonthlyFee(101, 2026, 10));

    // Invalid billing month must fail
    assert(!ledger.generateMonthlyFee(101, 2026, 13));

    const auto& obligations = ledger.getObligations();

    assert(obligations.size() == 2);

    assert(obligations[0].childId == 101);
    assert(obligations[0].billingYear == 2026);
    assert(obligations[0].billingMonth == 11);
    assert(obligations[0].amountCents == 15000);

    assert(obligations[0].dueDate.year == 2026);
    assert(obligations[0].dueDate.month == 11);
    assert(obligations[0].dueDate.day == 15);

    assert(obligations[1].billingMonth == 12);
    assert(obligations[1].dueDate.day == 15);
    assert(obligations[1].amountCents == 15000);


    return 0;
}
