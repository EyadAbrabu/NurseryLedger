#ifndef CLI_HPP
#define CLI_HPP


#include "Ledger.hpp"
#include <string>
#include <sstream>
#include <cstdint>


class CLI {
public:
    explicit CLI(Ledger& ledger);
    void run();

private:
    Ledger& ledger;

    void showHelp() const;
    void executeCommand(const std::string& line);


    void handleRegister(std::istringstream& input);
    void handleChildren() const;
    void handleBill(std::istringstream& input);
    static bool parseMoney(const std::string& text,
                       std::int64_t& cents);

    void handlePay(std::istringstream& input);
    void handleBalance(std::istringstream& input);
    void handleCredit(std::istringstream& input);
    void handleRefund(std::istringstream& input);


};

#endif
