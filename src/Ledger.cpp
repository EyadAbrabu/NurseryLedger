#include "Ledger.hpp"
#include "FeeObligation.hpp"

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
bool Ledger::generateMonthlyFee(int childId, int year, int month) {
    // 1. Validate the billing period
    if (year < 1 || month < 1 || month > 12)
        return false;

    // 2. Find the registered child
    for (const Child& child : children) {
        if (child.id != childId)
            continue;

        // 3. Reject billing before enrollment
        if (year < child.enrollmentDate.year ||
            (year == child.enrollmentDate.year &&
             month < child.enrollmentDate.month))
            return false;

        // 4. Prevent duplicate monthly billing (FR-13)
        for (const FeeObligation& obligation : obligations) {
            if (obligation.childId == childId &&
                obligation.billingYear == year &&
                obligation.billingMonth == month)
                return false;
        }

        // 5. Create the monthly obligation
        FeeObligation obligation{
            childId,
            year,
            month,
            {year, month, child.enrollmentDate.day},
            child.monthlyFeeCents
        };

        obligations.push_back(obligation);
        return true;
    }

    // Child does not exist
    return false;
}


const std::vector<FeeObligation>& Ledger::getObligations() const {
    return obligations;
}


bool Ledger::recordPayment(const Payment& payment) {
    if (!payment.receivedDate.isValid())
        return false;

    if (payment.amountCents <= 0)
        return false;

    if (payment.billingYear < 1 ||
        payment.billingMonth < 1 ||
        payment.billingMonth > 12)
        return false;

    bool childExists = false;

    for (const Child& child : children) {
        if (child.id == payment.childId) {
            childExists = true;

            if (payment.billingYear < child.enrollmentDate.year ||
                (payment.billingYear == child.enrollmentDate.year &&
                 payment.billingMonth < child.enrollmentDate.month))
                return false;

            break;
        }
    }

    if (!childExists)
        return false;

    bool obligationExists = false;

    for (const FeeObligation& obligation : obligations) {
        if (obligation.childId == payment.childId &&
            obligation.billingYear == payment.billingYear &&
            obligation.billingMonth == payment.billingMonth) {
            obligationExists = true;
            break;
        }
    }

    if (!obligationExists)
        return false;

    payments.push_back(payment);
    return true;
}

const std::vector<Payment>& Ledger::getPayments() const {
    return payments;
}


std::int64_t Ledger::getOutstandingBalance(
    int childId,
    int year,
    int month
) const {
    std::int64_t amountDue = 0;
    std::int64_t amountPaid = 0;

    for (const FeeObligation& obligation : obligations) {
        if (obligation.childId == childId &&
            obligation.billingYear == year &&
            obligation.billingMonth == month) {
            amountDue += obligation.amountCents;
        }
    }

    for (const Payment& payment : payments) {
        if (payment.childId == childId &&
            payment.billingYear == year &&
            payment.billingMonth == month) {
            amountPaid += payment.amountCents;
        }
    }

    return amountDue > amountPaid
        ? amountDue - amountPaid
        : 0;
}
