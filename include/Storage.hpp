#ifndef STORAGE_HPP
#define STORAGE_HPP

#include "Ledger.hpp"

class Storage {
public:
    bool save(const Ledger& ledger);
    bool load(Ledger& ledger);
};

#endif
