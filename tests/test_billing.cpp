#include "TestCheck.hpp"
#include "Ledger.hpp"

int main() {
    Ledger ledger;

    Child ahmed{101, "Ahmed", {2026, 11, 15}, 15000};
    CHECK(ledger.registerChild(ahmed));

    // First monthly fee must succeed
    CHECK(ledger.generateMonthlyFee(101, 2026, 11));

    // Duplicate monthly fee must fail
    CHECK(!ledger.generateMonthlyFee(101, 2026, 11));

    // Next month's fee must succeed
    CHECK(ledger.generateMonthlyFee(101, 2026, 12));

    // Unknown child must fail
    CHECK(!ledger.generateMonthlyFee(999, 2026, 11));

    // Billing before enrollment must fail
    CHECK(!ledger.generateMonthlyFee(101, 2026, 10));

    // Invalid billing month must fail
    CHECK(!ledger.generateMonthlyFee(101, 2026, 13));

    const auto& obligations = ledger.getObligations();

    CHECK(obligations.size() == 2);

    CHECK(obligations[0].childId == 101);
    CHECK(obligations[0].billingYear == 2026);
    CHECK(obligations[0].billingMonth == 11);
    CHECK(obligations[0].amountCents == 15000);

    CHECK(obligations[0].dueDate.year == 2026);
    CHECK(obligations[0].dueDate.month == 11);
    CHECK(obligations[0].dueDate.day == 15);

    CHECK(obligations[1].billingMonth == 12);
    CHECK(obligations[1].dueDate.day == 15);
    CHECK(obligations[1].amountCents == 15000);


    return 0;
}
