#ifndef LEDGER_HPP
#define LEDGER_HPP

#include <vector>
#include "Child.hpp"
#include "FeeObligation.hpp"

class Ledger {
private:
    std::vector<Child> children;
    std::vector<FeeObligation> obligations;

public:
    bool registerChild(const Child& child);
    bool generateMonthlyFee(int childId, int year, int month);
    const std::vector<FeeObligation>& getObligations() const;
};

#endif
