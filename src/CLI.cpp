#include "CLI.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>


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


void CLI::handleRegister(std::istringstream& input) {
    int id, year, month, day;
    char dash1, dash2;
    std::string name, fee;

    if (!(input >> id >> name >> year >> dash1 >> month >> dash2 >> day >> fee)
        || dash1 != '-' || dash2 != '-') {
        std::cout << "Usage: register <id> <name> <YYYY-MM-DD> <fee>\n";
        return;
    }

    // Convert euros to cents without floating-point rounding.
    std::int64_t euros, cents;
    char dot;
    std::istringstream money(fee);

    if (!(money >> euros >> dot >> cents) || dot != '.' ||
        cents < 0 || cents > 99) {
        std::cout << "Invalid fee. Use format 150.00\n";
        return;
    }

    Child child{id, name, {year, month, day}, euros * 100 + cents};

    if (ledger.registerChild(child)) {
        std::cout << "Child registered successfully.\n";
    } else {
        std::cout << "Registration failed. Check the ID, date, or fee.\n";
    }
}

void CLI::handleChildren() const {
    const auto& children = ledger.getChildren();

    if (children.empty()) {
        std::cout << "No children registered.\n";
        return;
    }

    std::cout << "\nID\tName\tEnrollment\tMonthly Fee\n";

    for (const auto& child : children) {
        std::cout << child.id << '\t'
                  << child.name << '\t'
                  << child.enrollmentDate.year << '-'
                  << child.enrollmentDate.month << '-'
                  << child.enrollmentDate.day << '\t'
                  << child.monthlyFeeCents / 100 << '.'
		  << std::setw(2) << std::setfill('0')
		  << child.monthlyFeeCents % 100 << " EUR\n";
    }
}

void CLI::handleBill(std::istringstream& input) {
    int childId, year, month;

    // Read the child ID and billing period from the terminal.
    // Ledger remains responsible for validating the business rules.
    if (!(input >> childId >> year >> month)) {
        std::cout << "Usage: bill <child_id> <year> <month>\n";
        return;
    }

    // Reject extra arguments rather than silently ignoring them.
    std::string extra;
    if (input >> extra) {
        std::cout << "Too many arguments. Usage: bill <child_id> <year> <month>\n";
        return;
    }

    // Delegate fee generation to Ledger, which checks whether the child
    // exists, whether the period is valid, and whether a bill already exists.
    if (ledger.generateMonthlyFee(childId, year, month)) {
        std::cout << "Monthly fee generated successfully for child "
                  << childId << " (" << year << "-" << month << ").\n";
    } else {
        std::cout << "Billing failed. Check the child ID, billing period, "
                  << "enrollment date, or duplicate fee.\n";
    }
}





void CLI::executeCommand(const std::string& line) {
    std::istringstream input(line);
    std::string command;
    input >> command;

    if (command == "help") {
        showHelp();
    } else if (command == "register") {
        handleRegister(input);
    } else if (command == "children") {
        handleChildren();
    } else if (command == "bill") {
    handleBill(input); 

   } else if (command.empty()) {
        return;
    } else {
        std::cout << "Unknown or unimplemented command: "
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
