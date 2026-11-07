#include "Storage.hpp"
#include <fstream>
#include <sstream>
#include <string>
#include <filesystem>
#include <utility>
#include <chrono>


bool Storage::save(const Ledger& ledger) {
    return saveSnapshot(ledger);
}

bool Storage::writeFiles(
    const Ledger& ledger,
    const std::filesystem::path& directory
) {

   std::ofstream file(directory / "children.csv");

    if (!file.is_open()) {
        return false;
    }

    file << "child_id,name,year,month,day,monthly_fee_cents\n";

    for (const Child& child : ledger.getChildren()) {
        file << child.id << ","
             << child.name << ","
             << child.enrollmentDate.year << ","
             << child.enrollmentDate.month << ","
             << child.enrollmentDate.day << ","
             << child.monthlyFeeCents << "\n";
    }

    // Check that the children CSV was written successfully.
file.close();

if (file.fail()) {
    return false;
}

// Save generated monthly fee obligations separately.
std::ofstream billsFile(directory / "obligations.csv");

if (!billsFile.is_open()) {
    return false;
}

billsFile << "child_id,billing_year,billing_month,due_year,due_month,due_day,amount_cents\n";

for (const FeeObligation& bill : ledger.getObligations()) {
    billsFile << bill.childId << ","
              << bill.billingYear << ","
              << bill.billingMonth << ","
              << bill.dueDate.year << ","
              << bill.dueDate.month << ","
              << bill.dueDate.day << ","
              << bill.amountCents << "\n";
}

billsFile.close();

if (billsFile.fail()) {
    return false;
}

// Save payment records.
std::ofstream paymentsFile(directory / "payments.csv");

if (!paymentsFile.is_open()) {
    return false;
}

paymentsFile << "child_id,year,month,day,amount_cents,method,billing_year,billing_month\n";

for (const Payment& payment : ledger.getPayments()) {
    paymentsFile << payment.childId << ","
                 << payment.receivedDate.year << ","
                 << payment.receivedDate.month << ","
                 << payment.receivedDate.day << ","
                 << payment.amountCents << ","
                 << (payment.method == PaymentMethod::Cash ? "cash" : "bank") << ","
                 << payment.billingYear << ","
                 << payment.billingMonth << "\n";
}
paymentsFile.close();

if (paymentsFile.fail()) {
    return false;
}

// Save refunds separately from payments.
std::ofstream refundsFile(directory / "refunds.csv");

if (!refundsFile.is_open()) {
    return false;
}

refundsFile << "child_id,year,month,day,amount_cents,method,billing_year,billing_month\n";

for (const Refund& refund : ledger.getRefunds()) {
    refundsFile << refund.childId << ","
                << refund.issuedDate.year << ","
                << refund.issuedDate.month << ","
                << refund.issuedDate.day << ","
                << refund.amountCents << ","
                << (refund.method == PaymentMethod::Cash ? "cash" : "bank") << ","
                << refund.billingYear << ","
                << refund.billingMonth << "\n";
}

refundsFile.close();
return !refundsFile.fail();

}



bool Storage::saveSnapshot(const Ledger& ledger) {
    namespace fs = std::filesystem;

    try {
        const fs::path root = ".nurseryledger";
        const fs::path snapshots = root / "snapshots";

        fs::create_directories(snapshots);

        // Give each snapshot a unique directory name.
        const auto timestamp =
            std::chrono::steady_clock::now()
                .time_since_epoch().count();

        const std::string name =
            "snapshot-" + std::to_string(timestamp);

        const fs::path temporary =
            snapshots / (name + ".tmp");

        const fs::path completed =
            snapshots / name;

        // Never overwrite another snapshot.
        if (fs::exists(temporary) ||
            fs::exists(completed)) {
            return false;
        }

        fs::create_directory(temporary);

        // Write all four CSV files into the new directory.
        if (!writeFiles(ledger, temporary)) {
            fs::remove_all(temporary);
            return false;
        }

        // The snapshot is complete. Publish its directory.
        fs::rename(temporary, completed);

        // Prepare a pointer to the latest complete snapshot.
        const fs::path pointerTemp = root / "CURRENT.tmp";
        const fs::path pointer = root / "CURRENT";

        {
            std::ofstream out(pointerTemp, std::ios::trunc);

            if (!out.is_open()) {
                return false;
            }

            out << name << '\n';
            out.close();

            if (!out.good()) {
                return false;
            }
        }

        // On our Linux filesystem, this rename atomically
        // replaces the previous CURRENT pointer.
        fs::rename(pointerTemp, pointer);

        return true;
    }
    catch (const fs::filesystem_error&) {
        return false;
    }
}


bool Storage::load(Ledger& ledger) {
    namespace fs = std::filesystem;

    const fs::path root = ".nurseryledger";
    const fs::path pointer = root / "CURRENT";

    try {
        if (fs::exists(pointer)) {
            std::ifstream in(pointer);
            std::string snapshotName;
            std::string extra;

            if (!in ||
                !std::getline(in, snapshotName) ||
                snapshotName.empty() ||
                snapshotName == "." ||
                snapshotName == ".." ||
                snapshotName.find_first_not_of(
                    "abcdefghijklmnopqrstuvwxyz"
                    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                    "0123456789-_"
                ) != std::string::npos ||
                std::getline(in, extra)) {
                return false;
            }

            const fs::path directory =
                root / "snapshots" / snapshotName;

            if (!fs::is_directory(directory)) {
                return false;
            }

            // Every committed snapshot must be complete.
            for (const char* filename : {
                "children.csv",
                "obligations.csv",
                "payments.csv",
                "refunds.csv"
            }) {
                if (!fs::is_regular_file(directory / filename)) {
                    return false;
                }
            }

            return loadFiles(ledger, directory);
        }

        // A snapshot system without CURRENT is incomplete.
        // Do not silently fall back to older financial records.
        if (fs::exists(root)) {
            return false;
        }

        // Compatibility with the original CSV storage.
        return loadFiles(ledger, ".");
    }
    catch (const fs::filesystem_error&) {
        return false;
    }
}


bool Storage::loadFiles(
    Ledger& ledger,
    const std::filesystem::path& directory
) {
    std::ifstream file(	directory / "children.csv");
    Ledger temporaryLedger;
    if (!file.is_open()) {
    // A new installation has no existing financial records.
    // Missing children.csv alongside other CSV files indicates
    // an incomplete dataset, so refuse to start.
    const bool otherDataExists =
        std::filesystem::exists(directory / "obligations.csv") ||
        std::filesystem::exists(directory / "payments.csv") ||
        std::filesystem::exists(directory / "refunds.csv");

    if (otherDataExists) {
        return false;
    }

    return !std::filesystem::exists(directory / "children.csv");
}


    std::string line;
    std::getline(file, line); // Skip CSV header

    while (std::getline(file, line)) {
        std::istringstream row(line);
std::string field;
Child child;

// A child record must contain exactly six columns.
int commaCount = 0;

for (char character : line) {
    if (character == ',') {
        ++commaCount;
    }
}

if (commaCount != 5) {
    return false;
}



        try {
            std::getline(row, field, ',');
            child.id = std::stoi(field);

            std::getline(row, child.name, ',');

            std::getline(row, field, ',');
            child.enrollmentDate.year = std::stoi(field);

            std::getline(row, field, ',');
            child.enrollmentDate.month = std::stoi(field);

            std::getline(row, field, ',');
            child.enrollmentDate.day = std::stoi(field);

            std::getline(row, field, ',');
            child.monthlyFeeCents = std::stoll(field);

            if (!temporaryLedger.registerChild(child)) {
    return false;
}
        }
        catch (const std::exception&) {
            return false;
        }
    }

     if (file.bad()) {
    return false;
}

// Load previously generated monthly bills.
std::ifstream billsFile(directory /"obligations.csv");

if (billsFile.is_open()) {
    std::string billLine;

    // Skip CSV header.
    if (!std::getline(billsFile, billLine)) {
        return false;
    }

    while (std::getline(billsFile, billLine)) {
        std::istringstream row(billLine);
        std::string field;
        int values[7];

        try {
            // Each bill must contain exactly seven integer columns.
            for (int i = 0; i < 7; ++i) {
                if (!std::getline(row, field, ',')) {
                    return false;
                }

                std::size_t consumed = 0;
                values[i] = std::stoi(field, &consumed);

                if (consumed != field.size()) {
                    return false;
                }
            }

            // Reject extra columns.
            if (std::getline(row, field, ',')) {
                return false;
            }

            const int childId = values[0];
            const int year = values[1];
            const int month = values[2];

            // Recreate the bill through the existing Ledger rules.
            if (!temporaryLedger.generateMonthlyFee(childId, year, month)) {
                return false;
            }

            // Verify that the generated bill matches the saved record.
            bool matches = false;

            for (const auto& bill : temporaryLedger.getObligations()) {
                if (bill.childId == childId &&
                    bill.billingYear == year &&
                    bill.billingMonth == month &&
                    bill.dueDate.year == values[3] &&
                    bill.dueDate.month == values[4] &&
                    bill.dueDate.day == values[5] &&
                    bill.amountCents == values[6]) {
                    matches = true;
                    break;
                }
            }

            if (!matches) {
                return false;
            }
        }
        catch (const std::exception&) {
            return false;
        }
    }

    if (billsFile.bad()) {
        return false;
    }
}
else if (std::filesystem::exists(directory / "obligations.csv")) {
    return false;
}


// Restore payments after children and bills have been loaded.
std::ifstream paymentsFile(directory /"payments.csv");

if (paymentsFile.is_open()) {
    std::string paymentLine;

    // Validate the CSV header.
    if (!std::getline(paymentsFile, paymentLine) ||
        paymentLine !=
            "child_id,year,month,day,amount_cents,method,billing_year,billing_month") {
        return false;
    }

    while (std::getline(paymentsFile, paymentLine)) {
        std::istringstream row(paymentLine);
        std::string fields[8];

        // Read exactly eight columns.
        for (int i = 0; i < 8; ++i) {
            if (!std::getline(row, fields[i], ',')) {
                return false;
            }
        }

        // Reject extra columns.
        std::string extra;
        if (std::getline(row, extra, ',')) {
            return false;
        }

        try {
            // Reject numbers with extra characters, such as "101abc".
            auto parseInt = [](const std::string& value) {
                std::size_t used = 0;
                int result = std::stoi(value, &used);

                if (used != value.size()) {
                    throw std::invalid_argument("Invalid integer");
                }

                return result;
            };

            auto parseCents = [](const std::string& value) {
                std::size_t used = 0;
                std::int64_t result = std::stoll(value, &used);

                if (used != value.size()) {
                    throw std::invalid_argument("Invalid amount");
                }

                return result;
            };

            Payment payment;

            payment.childId = parseInt(fields[0]);
            payment.receivedDate = {
                parseInt(fields[1]),
                parseInt(fields[2]),
                parseInt(fields[3])
            };

            payment.amountCents = parseCents(fields[4]);

            if (fields[5] == "cash") {
                payment.method = PaymentMethod::Cash;
            }
            else if (fields[5] == "bank") {
                payment.method = PaymentMethod::BankTransfer;
            }
            else {
                return false;
            }

            payment.billingYear = parseInt(fields[6]);
            payment.billingMonth = parseInt(fields[7]);

            // Reuse Ledger validation instead of inserting records directly.
            if (!temporaryLedger.recordPayment(payment)) {
                return false;
            }
        }
        catch (const std::exception&) {
            return false;
        }
    }

    if (paymentsFile.bad()) {
        return false;
    }
}
else if (std::filesystem::exists(directory / "payments.csv")) {
    return false;
}


// Restore refunds after payments so available credit can be validated.
std::ifstream refundsFile(directory / "refunds.csv");

if (refundsFile.is_open()) {
    std::string refundLine;

    if (!std::getline(refundsFile, refundLine) ||
        refundLine !=
            "child_id,year,month,day,amount_cents,method,billing_year,billing_month") {
        return false;
    }

    while (std::getline(refundsFile, refundLine)) {
        std::istringstream row(refundLine);
        std::string fields[8];

        for (int i = 0; i < 8; ++i) {
            if (!std::getline(row, fields[i], ',')) {
                return false;
            }
        }

        std::string extra;
        if (std::getline(row, extra, ',')) {
            return false;
        }

        try {
            auto parseInt = [](const std::string& value) {
                std::size_t used = 0;
                int result = std::stoi(value, &used);

                if (used != value.size()) {
                    throw std::invalid_argument("Invalid integer");
                }

                return result;
            };

            auto parseCents = [](const std::string& value) {
                std::size_t used = 0;
                std::int64_t result = std::stoll(value, &used);

                if (used != value.size()) {
                    throw std::invalid_argument("Invalid amount");
                }

                return result;
            };

            Refund refund;

            refund.childId = parseInt(fields[0]);

            refund.issuedDate = {
                parseInt(fields[1]),
                parseInt(fields[2]),
                parseInt(fields[3])
            };

            refund.amountCents = parseCents(fields[4]);

            if (fields[5] == "cash") {
                refund.method = PaymentMethod::Cash;
            }
            else if (fields[5] == "bank") {
                refund.method = PaymentMethod::BankTransfer;
            }
            else {
                return false;
            }

            refund.billingYear = parseInt(fields[6]);
            refund.billingMonth = parseInt(fields[7]);

            // Existing Ledger rules prevent refunding consumed credit.
            if (!temporaryLedger.recordRefund(refund)) {
                return false;
            }
        }
        catch (const std::exception&) {
            return false;
        }
    }

    if (refundsFile.bad()) {
        return false;
    }
}
else if (std::filesystem::exists(directory / "refunds.csv")) {
    return false;
}


// Accept all records only after successful loading.
ledger = std::move(temporaryLedger);
return true;
}
