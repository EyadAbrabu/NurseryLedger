#include <iostream>
#include "Date.hpp"
#include "CLI.hpp"
#include "Ledger.hpp"

int main() {
    Ledger ledger;
    CLI cli(ledger);

    cli.run();

    return 0;
}
