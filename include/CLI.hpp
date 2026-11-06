#ifndef CLI_HPP
#define CLI_HPP


#include "Ledger.hpp"
#include <string>
#include <sstream>



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


};

#endif
