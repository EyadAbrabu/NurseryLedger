#include "CLI.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cctype>
#include <limits>

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
    std::int64_t amountCents;

if (!parseMoney(fee, amountCents)) {
    std::cout << "Invalid fee. Use format 150.00\n";
    return;
}


    Child child{id, name, {year, month, day}, amountCents};;

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

void CLI::handlePay(std::istringstream& input) {
    int childId, year, month;
    std::string dateText, amountText, methodText;

    // Parse the payment details. The billing period is separate
    // from the date the payment was actually received.
    if (!(input >> childId >> dateText >> amountText
                >> methodText >> year >> month)) {
        std::cout << "Usage: pay <child_id> <YYYY-MM-DD> "
                     "<amount> <cash|bank> <year> <month>\n";
        return;
    }

    std::string extra;
    if (input >> extra) {
        std::cout << "Too many arguments.\n";
        return;
    }

    // Validate the date format before constructing a Date.
    int receivedYear, receivedMonth, receivedDay;
    char dash1, dash2;
    std::istringstream dateInput(dateText);

    if (!(dateInput >> receivedYear >> dash1
                    >> receivedMonth >> dash2 >> receivedDay)
        || dash1 != '-' || dash2 != '-'
        || dateInput.peek() != std::char_traits<char>::eof()) {
        std::cout << "Invalid date. Use YYYY-MM-DD.\n";
        return;
    }

    Date receivedDate{receivedYear, receivedMonth, receivedDay};

    if (!receivedDate.isValid()) {
        std::cout << "Invalid calendar date.\n";
        return;
    }

    // Use the shared parser so all financial commands interpret
    // monetary values consistently without floating-point errors.
    std::int64_t amountCents;

    if (!parseMoney(amountText, amountCents)) {
        std::cout << "Invalid amount. Use format 150.00\n";
        return;
    }

    PaymentMethod method;

    if (methodText == "cash") {
        method = PaymentMethod::Cash;
    } else if (methodText == "bank") {
        method = PaymentMethod::BankTransfer;
    } else {
        std::cout << "Invalid payment method. Use cash or bank.\n";
        return;
    }

    Payment payment{
        childId,
        receivedDate,
        amountCents,
        method,
        year,
        month
    };

    // The Ledger validates the child, billing period, and
    // obligation before accepting the payment.
    if (ledger.recordPayment(payment)) {
        std::cout << "Payment recorded successfully.\n";
    } else {
        std::cout << "Payment rejected. Check the child ID, "
                     "billing period, or existing obligation.\n";
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
    } else if (command == "pay") {
    handlePay(input);
    } else if (command == "balance") {
    handleBalance(input);
   } else if (command == "credit") {
      handleCredit(input);
    } else if (command == "refund") {
    handleRefund(input);

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


bool CLI::parseMoney(const std::string& text,
                     std::int64_t& cents) {
    // Require a non-negative amount with exactly two decimal
    // places, such as 150.00. Never use floating-point money.
    const auto dot = text.find('.');

    if (dot == std::string::npos ||
        dot == 0 ||
        dot != text.size() - 3) {
        return false;
    }

    for (std::size_t i = 0; i < text.size(); ++i) {
        if (i == dot) continue;

        if (!std::isdigit(
                static_cast<unsigned char>(text[i]))) {
            return false;
        }
    }

    std::int64_t euros = 0;

    // Check overflow before multiplying or adding digits.
    const auto maxEuros =
        std::numeric_limits<std::int64_t>::max() / 100;

    for (std::size_t i = 0; i < dot; ++i) {
        int digit = text[i] - '0';

        if (euros > (maxEuros - digit) / 10) {
            return false;
        }

        euros = euros * 10 + digit;
    }

    const int fractional =
        (text[dot + 1] - '0') * 10 +
        (text[dot + 2] - '0');

    if (euros >
        (std::numeric_limits<std::int64_t>::max()
         - fractional) / 100) {
        return false;
    }

    cents = euros * 100 + fractional;
    return cents > 0;
}

void CLI::handleBalance(std::istringstream& input) {
    int childId, year, month;
    std::string extra;

    // The balance belongs to one child and one billing period.
    if (!(input >> childId >> year >> month) || (input >> extra)) {
        std::cout << "Usage: balance <child_id> <year> <month>\n";
        return;
    }

    // Verify that the requested obligation exists. A missing bill
    // should not be confused with an existing bill that is fully paid.
    bool found = false;

    for (const auto& obligation : ledger.getObligations()) {
        if (obligation.childId == childId &&
            obligation.billingYear == year &&
            obligation.billingMonth == month) {
            found = true;
            break;
        }
    }

    if (!found) {
        std::cout << "No bill exists for this child and period.\n";
        return;
    }

    const auto cents =
        ledger.getOutstandingBalance(childId, year, month);

    std::cout << "Outstanding balance: "
              << cents / 100 << '.'
              << std::setw(2) << std::setfill('0')
              << cents % 100 << " EUR\n";
}

void CLI::handleCredit(std::istringstream& input) {
    int childId;
    std::string extra;

    if (!(input >> childId) || (input >> extra)) {
        std::cout << "Usage: credit <child_id>\n";
        return;
    }

    // Check child existence so an unknown ID is not reported
    // as a valid child with zero available credit.
    bool found = false;

    for (const auto& child : ledger.getChildren()) {
        if (child.id == childId) {
            found = true;
            break;
        }
    }

    if (!found) {
        std::cout << "Child not found.\n";
        return;
    }

    // Credit allocation is calculated entirely by the Ledger.
    const auto cents = ledger.getAvailableCredit(childId);

    std::cout << "Available credit: "
              << cents / 100 << '.'
              << std::setw(2) << std::setfill('0')
              << cents % 100 << " EUR\n";
}
void CLI::handleRefund(std::istringstream& input) {
    int childId, year, month;
    std::string dateText, amountText, methodText;

    // Refunds reference the original billing period that produced
    // the credit. The issue date records when money was returned.
    if (!(input >> childId >> dateText >> amountText
                >> methodText >> year >> month)) {
        std::cout << "Usage: refund <child_id> <YYYY-MM-DD> "
                     "<amount> <cash|bank> <year> <month>\n";
        return;
    }

    std::string extra;
    if (input >> extra) {
        std::cout << "Too many arguments.\n";
        return;
    }

    // Parse the refund issue date and reject malformed input.
    int refundYear, refundMonth, refundDay;
    char dash1, dash2;
    std::istringstream dateInput(dateText);

    if (!(dateInput >> refundYear >> dash1
                    >> refundMonth >> dash2 >> refundDay)
        || dash1 != '-' || dash2 != '-'
        || dateInput.peek() != std::char_traits<char>::eof()) {
        std::cout << "Invalid date. Use YYYY-MM-DD.\n";
        return;
    }

    Date issuedDate{refundYear, refundMonth, refundDay};

    if (!issuedDate.isValid()) {
        std::cout << "Invalid calendar date.\n";
        return;
    }

    // Reuse the same strict money parser used for registration
    // and payments, avoiding floating-point calculations.
    std::int64_t amountCents;

    if (!parseMoney(amountText, amountCents)) {
        std::cout << "Invalid amount. Use format 30.00\n";
        return;
    }

    PaymentMethod method;

    if (methodText == "cash") {
        method = PaymentMethod::Cash;
    } else if (methodText == "bank") {
        method = PaymentMethod::BankTransfer;
    } else {
        std::cout << "Invalid refund method. Use cash or bank.\n";
        return;
    }

    Refund refund{
        childId,
        issuedDate,
        amountCents,
        method,
        year,
        month
    };

    // Ledger enforces the refund policy: only unused credit
    // originating from this billing period can be refunded.
    // Credit already applied to later bills cannot be refunded.
    if (ledger.recordRefund(refund)) {
        std::cout << "Refund recorded successfully.\n";
    } else {
        std::cout << "Refund rejected. Check the child, billing period, "
                     "and available refundable credit.\n";
    }
}
