#ifndef LEDGER_HPP
#define LEDGER_HPP

#include <vector>
#include "Child.hpp"
#include "FeeObligation.hpp"
#include "Payment.hpp"
#include "Refund.hpp"


class Ledger {
private:
    std::vector<Child> children;
    std::vector<FeeObligation> obligations;
    std::vector<Payment> payments;
    std::vector<Refund> refunds;



public:
    bool registerChild(const Child& child);
    bool generateMonthlyFee(int childId, int year, int month);

    const std::vector<FeeObligation>& getObligations() const;

    bool recordPayment(const Payment& payment);
    const std::vector<Payment>& getPayments() const;


    std::int64_t getOutstandingBalance(
    int childId,
    int year,
    int month
) const;


    std::int64_t getAvailableCredit(int childId) const;


    bool recordRefund(const Refund& refund);

const std::vector<Refund>& getRefunds() const;




};

#endif
