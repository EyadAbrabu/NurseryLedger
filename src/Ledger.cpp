#include "Ledger.hpp"

bool Ledger::registerChild(const Child& child) {

    // Validate child ID and name
    if (child.id <= 0 || child.name.empty()) {
        return false;
    }

    // Validate the enrollment date
    if (!child.enrollmentDate.isValid()) {
        return false;
    }

    // Enrollment is only allowed on the 1st or 15th
    if (child.enrollmentDate.day != 1 &&
        child.enrollmentDate.day != 15) {
        return false;
    }

    // Monthly fee must be positive
    if (child.monthlyFeeCents <= 0) {
        return false;
    }

    // Prevent duplicate child IDs
    for (const Child& existingChild : children) {
        if (existingChild.id == child.id) {
            return false;
        }
    }

    // Register the child
    children.push_back(child);

    return true;
}
