#include "CLI.hpp"
#include "Ledger.hpp"
#include "Storage.hpp"

#include <iostream>

int main() {
    Ledger ledger;
    Storage storage;

    if (!storage.load(ledger)) {
    std::cerr << "Error: Unable to load saved data.\n";
    std::cerr << "Startup aborted to protect existing records.\n";
    return 1;
}

    CLI cli(ledger);
    cli.run();

    if (storage.save(ledger)) {
        std::cout << "Data saved successfully.\n";
    } else {
        std::cerr << "Error: Failed to save data.\n";
        return 1;
    }

    return 0;
}
