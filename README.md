# NurseryLedger

**A Linux-based C++17 command-line application for managing nursery fees, payments, outstanding balances, and financial records.**

A focused first version of a larger nursery management system, inspired by a real family business and built with an emphasis on financial correctness, testing, and data reliability.

**Tech Stack:** C++17 · Linux · CMake · Python 3 · CSV · Git

---

## 1. Overview

NurseryLedger is a financial management application designed around one of the most important responsibilities of running a nursery: **keeping track of the money that parents owe and the money the nursery actually receives.**

The application handles child registration, monthly fee obligations, payments, partial payments, overpayments, credit allocation, refunds, and financial history.

Rather than simply storing payment amounts, NurseryLedger applies a set of business rules to ensure that financial transactions are recorded consistently.

This is **Version 1 of a larger idea**. It focuses specifically on nursery fee management. Employee payroll, inventory, operational expenses, and advanced financial reporting belong to the future roadmap.

## 2. The Story Behind NurseryLedger

This project means a little more to me than most of the projects I've worked on.

My family once owned a nursery. My grandfather contributed financially to the business, and my mother worked incredibly hard to overcome its financial difficulties and keep it operating. It eventually became a source of support for our family before sadly closing in 2020.

I was still young when the nursery started, but I remember quite a lot from those years.

One particular memory was a situation involving missing bills. It caused considerable stress, and money had to be covered out of pocket until the issue was resolved.

When I received this development assignment, my first idea was actually a volunteer management application, inspired by my experience organizing developer community events.

But after spending a considerable amount of time brainstorming, I wanted to build something different. Something connected to a real problem, with business rules that I could explore and challenge.

That's how NurseryLedger started.

I wanted to revisit something from my childhood, this time from the perspective of a computer science student trying to build software that could make those administrative responsibilities easier.

## 3. Project Scope — Version 1

The long-term vision is a nursery management system connecting financial, administrative, and operational activities.

However, I decided that the first release should focus on one well-defined area: **managing nursery fees and payments**.

### Implemented functionality

| Area | Functionality |
|---|---|
| Child registration | Register children with unique IDs, enrollment dates, and monthly fees |
| Monthly billing | Generate fee obligations and prevent duplicate bills |
| Payment management | Record payments made by cash or bank transfer |
| Outstanding balances | Calculate remaining amounts for partially or unpaid obligations |
| Overpayments | Track unused payment credit |
| Credit allocation | Apply available credit toward later monthly obligations |
| Refunds | Refund eligible unused credit without allowing over-refunding |
| Financial history | View recorded bills, payments, and refunds |
| Persistence | Save and restore financial records using versioned CSV snapshots |
| Validation | Reject invalid financial operations and inconsistent stored records |

### Business rules

Some rules were deliberately simplified to keep the first release predictable and testable.

- Enrollment is supported on the **1st or 15th** of the month.
- Monthly fee obligations follow the child's enrollment-day billing anchor.
- A child cannot receive two bills for the same billing period.
- A payment is associated with a billing period, while its actual receipt date is recorded separately.
- Partial payments leave an outstanding balance.
- Overpayments create available credit that can be applied toward later obligations.
- Refunds are limited to eligible unused credit from the referenced billing period.
- Monetary values are stored as **64-bit integer cents**, rather than floating-point numbers.

One of the most important decisions was separating the **billing period** from the **payment receipt date**.

For example, a fee due for November might not be paid until December. Preserving both dates makes it possible to distinguish what the nursery was owed from when it actually received the money.

This is also a useful foundation for more advanced financial reporting in future releases.

## 4. Getting Started

### Requirements

- Linux (developed and tested on Ubuntu)
- A C++ compiler supporting C++17
- CMake 3.16 or newer
- Python 3 for integration testing
- Git for cloning the repository

### Clone the repository

```bash
git clone https://github.com/EyadAbrabu/NurseryLedger.git
cd NurseryLedger
```

### Build the application

```bash
cmake -S . -B build
cmake --build build
```

### Run NurseryLedger

```bash
./build/nurseryledger
```

The CLI opens with a welcome screen. Enter `help` to see the available commands.

**Important:** Run the executable from the repository root, or another chosen working directory. Financial data is saved relative to the current working directory.

## 5. Example Workflow

Let's take a practical example.

Ahmed joins the nursery on November 15, 2026, with a monthly fee of €150.

His November obligation is generated, but the nursery receives a payment of €200.

The extra €50 should not disappear or be treated as additional income owed. It becomes available credit.

### Register a child

```text
register 101 Ahmed 2026-11-15 150.00
```

### Generate the monthly fee

```text
bill 101 2026 11
```

### Record a €200 payment

```text
pay 101 2026-11-20 200.00 cash 2026 11
```

### Check the balance and credit

```text
balance 101 2026 11
credit 101
```

The November obligation is fully covered, leaving **€50.00 in available credit**.

### Refund part of the unused credit

```text
refund 101 2026-11-21 30.00 cash 2026 11
credit 101
```

After the €30 refund, **€20.00 remains as available credit**.

This workflow demonstrates the relationship between billing, received payments, overpayments, and refunds.

### Other commands

| Command | Purpose |
|---|---|
| `children` | List registered children |
| `bills 101` | Display the child's generated bills |
| `payments 101` | Display payment history |
| `refunds 101` | Display refund history |
| `help` | Display all available commands and argument formats |
| `exit` | Save the ledger and exit normally |

Dates use `YYYY-MM-DD`, monetary input uses a format such as `150.00`, and payment methods are `cash` or `bank`.

Child names are currently entered as single command-line tokens.

**Use `exit` to close the application normally and save the latest records.** Unexpected termination may lose changes made since the previous successful save.

## 6. System Architecture

NurseryLedger separates user interaction, financial business logic, data representation, and persistence.

### Main components

| Component | Responsibility |
|---|---|
| `CLI` | Parses commands, validates input formats, and displays results |
| `Ledger` | Enforces registration, billing, payment, credit, and refund rules |
| `Child` | Represents a registered child and their monthly fee |
| `FeeObligation` | Represents a fee owed for a specific billing period |
| `Payment` | Represents an actual payment and its receipt date |
| `Refund` | Represents the return of eligible unused credit |
| `Date` | Validates calendar dates, including leap-year rules |
| `Storage` | Saves, restores, and validates persistent records |

### Application flow

```text
                 main.cpp
                     |
              Storage::load()
                     |
                 CLI::run()
                     |
               User commands
                     |
                 Ledger
                     |
            Financial rules
                     |
              In-memory data
                     |
              Storage::save()
                     |
              CSV snapshots
```

The CLI does not directly implement the financial calculations. It delegates operations to the Ledger, which applies the business rules.

This separation makes it easier to test the core behavior independently from the terminal and filesystem.

### Structures and classes

I used `struct` for straightforward data models such as children, payments, and fee obligations.

Classes are used for components that own behavior and enforce rules, particularly the Ledger.

This was also an opportunity for me to explore a design approach beyond the programming patterns I was already comfortable with.

## 7. Persistence and Data Integrity

Persistence turned out to be one of the most challenging parts of the project.

My initial implementation saved the ledger directly into four CSV files:

```text
children.csv
obligations.csv
payments.csv
refunds.csv
```

As I started questioning the reliability of the application, I realized that directly overwriting multiple financial files could leave inconsistent records if saving was interrupted.

### Versioned snapshots

The storage system was extended to save complete datasets in versioned snapshot directories.

A typical storage layout looks like this:

```text
.nurseryledger/
├── CURRENT
└── snapshots/
    ├── snapshot-.../
    │   ├── children.csv
    │   ├── obligations.csv
    │   ├── payments.csv
    │   └── refunds.csv
    └── snapshot-.../
        ├── children.csv
        ├── obligations.csv
        ├── payments.csv
        └── refunds.csv
```

The application writes a new snapshot separately before updating the `CURRENT` pointer to identify the committed version.

On startup, NurseryLedger loads the selected snapshot and validates the records using the Ledger's existing rules.

The storage layer also supports loading the original root-level CSV format when no snapshot storage has been initialized.

### Failure handling

The implementation includes checks for incomplete or corrupted financial datasets.

For example:

- A missing `children.csv` alongside existing payment or billing records is treated as an incomplete dataset, not a new installation.
- An uncommitted temporary snapshot is ignored when loading a previously committed version.
- If `CURRENT` references an incomplete snapshot, startup is rejected.
- Invalid records are not silently accepted into the active Ledger.

During development, I encountered a particularly important loading bug. A payment was correctly written to a snapshot, but the loading process was still using the old CSV paths.

After restarting, the child appeared, but the payment and available credit did not.

I investigated the saved records, corrected the file paths, and verified that the €200 payment and its €50 credit could be restored.

That was an important reminder that **saving successfully is only half of persistence; restoring the correct data matters just as much.**

The current snapshot mechanism improves process-interruption recovery, but does not guarantee full durability against sudden power loss. It also does not automatically recover from every form of corrupted committed data.

## 8. Testing and Quality Assurance

Testing became one of my favorite parts of this project.

After working extensively with C++, returning to Python for integration testing felt refreshing. More importantly, it encouraged me to stop thinking only about successful operations and start asking what could go wrong.

I approached testing from two perspectives.

### C++ unit tests

Six test suites exercise the core business logic:

| Test suite | Focus |
|---|---|
| DateValidation | Date ranges and calendar validation |
| LedgerRegistration | Child registration rules |
| MonthlyBilling | Fee generation and duplicate billing |
| PaymentRecording | Payment validation and recording |
| CreditAllocation | Overpayments and credit allocation |
| RefundHandling | Refund limits and financial rules |

Run the C++ tests using:

```bash
ctest --test-dir build --output-on-failure
```

### Python component tests

Seventeen Python integration scenarios run the compiled application externally and check its behavior and persisted data.

These tests cover scenarios including:

- Records surviving application restarts.
- Rejection of duplicate child IDs and monthly bills.
- Rejection of payments without existing obligations.
- Partial payments and outstanding balances.
- Overpayments and credit carry-forward.
- Valid and invalid refunds.
- Prevention of refunds against already-consumed credit.
- Rejection of invalid monetary input.
- Preservation of existing records after failed operations.
- Detection of corrupted or incomplete CSV datasets.
- Consistency across repeated restarts.
- Recovery from simulated incomplete snapshot states.

Run them using:

```bash
python3 tests/integration/test_cli.py
```

### Complete verification

```bash
cmake --build build && \
ctest --test-dir build --output-on-failure && \
python3 tests/integration/test_cli.py
```

The latest development verification completed successfully with **6 C++ test suites and 17 Python integration scenarios passing**.

The test suite is designed to validate important financial rules and failure scenarios. It is not intended as proof that every possible defect, operational failure, or scalability problem has been eliminated.

## 9. Known Limitations and Open Engineering Questions

One thing this project taught me is that an application can work correctly in tested scenarios and still have important limitations in a real operating environment.

I started keeping track of these questions rather than treating every possible future improvement as something that had to be implemented immediately.

| Area | Current limitation or open question |
|---|---|
| Automatic persistence | Changes are saved on normal exit; Ctrl+C or other unexpected termination can lose unsaved changes |
| Payment idempotency | Duplicate monthly bills are prevented, but identical payment submissions are not independently identified as duplicate transactions |
| Power-loss durability | Snapshot operations are not guaranteed durable across sudden power loss |
| Data recovery | Certain corrupted datasets cause startup to be rejected and require manual recovery |
| Concurrency | The application is designed around a local single-user workflow |
| Storage format | CSV is simple and inspectable but has limitations for complex data and concurrent updates |
| Advanced billing | Mid-month withdrawals, fee changes, discounts, and irregular enrollment need additional business rules |
| Security | Authentication, authorization, audit logs, and protection of sensitive records are not implemented |
| Performance | Large-scale datasets have not been benchmarked |

### A few questions I would investigate next

**What happens if the same payment is submitted twice?**

Preventing duplicate monthly bills doesn't prevent an employee from accidentally entering the same payment twice. A future version should introduce transaction identifiers and idempotency rules.

**What happens when a child leaves halfway through the month?**

Should the nursery refund the unused period, charge the entire month, or apply a different policy? That is a business decision that should be defined before implementing the calculation.

**What happens when a parent has several overdue bills?**

The system needs a clearly defined policy for allocating payments and unused credit. More advanced payment allocation and correction workflows would be important for future releases.

**What happens when multiple administrators modify the same records?**

A larger system would need transactional operations and concurrency controls to prevent conflicting updates.

These questions are documented because I wanted the current release to have clear boundaries while recognizing what real-world use would demand.

## 10. Scalability — Beyond a Small Nursery

Another question that interested me during development was whether this design could eventually support a much larger organization.

What if a nursery grew to hundreds of children? Or what if the system was adapted for a school with thousands of students?

The honest answer is that **the current implementation has not been benchmarked at that scale**.

The present design loads records into memory and writes complete CSV snapshots. As the number of children, monthly obligations, and transactions grows, loading, searching, validation, and full-snapshot writing may become increasingly expensive.

For larger deployments, I would investigate:

- Performance benchmarks with realistic financial datasets.
- Database indexing and more efficient transaction queries.
- SQLite or another transactional database for stronger persistence guarantees.
- PostgreSQL or a similar database-backed service for larger multi-user deployments.
- Multi-user access, permissions, and audit trails.
- Backups, recovery procedures, and monitoring.
- A web interface suitable for administrators rather than a developer-oriented CLI.

A school would also introduce different financial requirements, such as semester fees, installment plans, discounts, and more complicated account management.

I see scalability as a question that must be answered with measurements and appropriate architecture, not an assumption based on the application working with a small dataset.

## 11. Long-Term Roadmap

NurseryLedger is intended to be the financial starting point of a broader nursery management platform.

| Phase | Focus | Status |
|---|---|---|
| Version 1 | Registration, fee obligations, payments, balances, credit, refunds, persistence and tests | **Implemented** |
| Financial reliability | Transactional persistence, automatic saving, payment idempotency, audit trails and recovery improvements | Planned |
| Advanced billing | Fee adjustments, discounts, flexible enrollment, payment corrections and reporting | Planned |
| Employee management | Salaries, employee advances, loans, repayments and payroll records | Future vision |
| Inventory and expenses | Food, cleaning materials, diapers, suppliers and operational expenses | Future vision |
| Integrated management | Financial dashboards, administrative roles, reporting and multi-user access | Future vision |

The larger product vision is intentionally separated from the functionality delivered in this release.

For the complete scope and planned development stages, see [ROADMAP.md](docs/ROADMAP.md).

## 12. Project Structure and Documentation

```text
NurseryLedger/
├── CMakeLists.txt
├── README.md
├── CONTRIBUTING.md
├── include/
│   ├── CLI.hpp
│   ├── Ledger.hpp
│   ├── Storage.hpp
│   └── ... data model headers
├── src/
│   ├── main.cpp
│   ├── CLI.cpp
│   ├── Ledger.cpp
│   ├── Storage.cpp
│   └── Date.cpp
├── tests/
│   ├── test_date.cpp
│   ├── test_ledger.cpp
│   ├── test_billing.cpp
│   ├── test_payment.cpp
│   ├── test_credit.cpp
│   ├── test_refund.cpp
│   └── integration/
│       └── test_cli.py
└── docs/
    ├── REQUIREMENTS.md
    ├── ROADMAP.md
    ├── DEVELOPMENT_LOG.md
    └── ENGINEERING_REFLECTION.md
```

**Supporting documentation**

- [Requirements](docs/REQUIREMENTS.md) — Functional and non-functional requirements.
- [Development Roadmap](docs/ROADMAP.md) — Current scope, future modules, and known limitations.
- [Development Log](docs/DEVELOPMENT_LOG.md) — Engineering decisions, implementation progress, and debugging history.
- [Contributor Guide](CONTRIBUTING.md) — Repository structure, development environment, testing, and contribution workflow.
- [Engineering Reflection](docs/ENGINEERING_REFLECTION.md) — A short essay about the project, technical decisions, organization, trade-offs, and what I learned.

## 13. Development Approach and AI Assistance

I used Git throughout development to document the project's progression, from initial planning and requirements to implementation, debugging, testing, and persistence improvements.

I also used AI assistance for brainstorming, discussing architecture, exploring unfamiliar C++ concepts, debugging errors, and developing testing strategies.

Suggestions were incorporated into the application through an iterative process of implementing, compiling, testing, examining failures, and making corrections.

One important lesson was that receiving a plausible solution is not the same as verifying it. Several problems during development reinforced the need to understand what the code actually does rather than assume a change is correct.

## 14. Final Reflection

Looking back, I think the biggest lesson from NurseryLedger was learning to think beyond the implementation itself.

At first, I was asking what I could build.

As the application progressed, I started asking different questions.

What happens if someone records the wrong payment? What if a file is corrupted? What if the application closes unexpectedly? What if a nursery grows into a school with thousands of students?

Not all those questions have answers in Version 1, and I don't want to pretend they do.

But asking them changed the way I approached the project.

I'm especially proud of reaching the testing stage after working through the C++ implementation. Writing the Python tests, exploring edge cases, and seeing the business rules work together was genuinely one of the most enjoyable parts.

I started this project with a childhood memory and an idea. I finished it with a working first version, a clearer understanding of its limitations, and a much stronger appreciation of the decisions involved in building dependable software.

**NurseryLedger started as a way to make an old problem easier to manage. It became an opportunity for me to learn how to approach that problem as a software engineer.**

---

### Release Readiness

NurseryLedger Version 1 delivers a complete implementation of its defined core scope: child registration, monthly billing, payment tracking, outstanding balances, credit allocation, refunds, and persistent financial records.

I approached development with production-quality engineering in mind, focusing on separating responsibilities, validating financial operations, testing important edge cases, and protecting stored records against identified failure scenarios.

The application has passed six C++ unit-test suites and seventeen Python integration scenarios, including tests for data corruption and snapshot recovery.

However, I distinguish between a functionally complete release and a system ready for unrestricted real-world deployment.

Before introducing NurseryLedger into daily operations with actual financial and personal data, I would prioritize automatic transaction persistence, duplicate-payment protection, access controls, and stronger backup and recovery guarantees.

These limitations are documented rather than hidden because I believe understanding a system's boundaries is part of responsible software engineering.

**Version 1 achieves its defined functional scope while establishing a foundation for further production hardening and expansion.**
