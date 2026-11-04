#include <cassert>
#include "Ledger.hpp"

int main() {

    Ledger ledger;

    // Test 1: Valid child registration
    Child child1{101, "Ahmed", {2026, 11, 15}, 15000};
    assert(ledger.registerChild(child1));

    // Test 2: Duplicate child ID
    Child duplicate{101, "Omar", {2026, 11, 1}, 12000};
    assert(!ledger.registerChild(duplicate));

    // Test 3: Invalid enrollment day
    Child invalidDay{102, "Sara", {2026, 11, 10}, 15000};
    assert(!ledger.registerChild(invalidDay));

    // Test 4: Invalid calendar date
    Child invalidDate{103, "Ali", {2026, 2, 31}, 15000};
    assert(!ledger.registerChild(invalidDate));

    // Test 5: Invalid monthly fee
    Child invalidFee{104, "Mona", {2026, 11, 1}, 0};
    assert(!ledger.registerChild(invalidFee));

    // Test 6: Empty name
    Child emptyName{105, "", {2026, 11, 15}, 15000};
    assert(!ledger.registerChild(emptyName));

    // Test 7: Invalid child ID
    Child invalidId{-1, "Youssef", {2026, 11, 1}, 15000};
    assert(!ledger.registerChild(invalidId));

    // Test 8: Another valid registration
    Child child2{106, "Layla", {2026, 11, 1}, 20000};
    assert(ledger.registerChild(child2));

    return 0;
}
