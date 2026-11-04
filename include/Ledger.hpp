#ifndef LEDGER_HPP
#define LEDGER_HPP

#include <vector>
#include "Child.hpp"

class Ledger {
private:
    std::vector<Child> children;

public:
    bool registerChild(const Child& child);
};

#endif
