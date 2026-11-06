#include "CLI.hpp"
#include <iostream>
#include <sstream>

CLI::CLI(Ledger& ledger) : ledger(ledger) {}

void CLI::showHelp() const {
    std::cout
        << "Available commands:\n"
        << "  help      Show commands\n"
        << "  register  Register a child\n"
        << "  children  List children\n"
        << "  bill      Generate monthly fee\n"
        << "  pay       Record payment\n"
        << "  refund    Record refund\n"
        << "  balance   Show outstanding balance\n"
        << "  credit    Show available credit\n"
        << "  payments  List payments\n"
        << "  refunds   List refunds\n"
        << "  exit      Quit\n";
}

void CLI::executeCommand(const std::string& line) {
    std::istringstream input(line);
    std::string command;
    input >> command;

    if (command == "help") {
        showHelp();
    } else if (command.empty()) {
        return;
    } else {
        std::cout << "Command not implemented yet: "
                  << command << '\n';
    }
}

void CLI::run() {
    std::string line;

    std::cout << "NurseryLedger CLI\n";
    showHelp();

    while (true) {
        std::cout << "nursery> ";

        if (!std::getline(std::cin, line))
            break;

        if (line == "exit")
            break;

        executeCommand(line);
    }

    std::cout << "Goodbye.\n";
}
