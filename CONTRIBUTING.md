# Contributing to NurseryLedger

Thank you for your interest in NurseryLedger!

This project started as a focused nursery fee-management application, with an emphasis on financial correctness, maintainable code, and testing.

This guide explains how to understand the repository, set up a development environment, run tests, and contribute without accidentally breaking the existing financial rules.

## 1. Development Environment

NurseryLedger targets Linux and uses C++17 for the application and Python 3 for component testing.

**Requirements:**
- Linux (Ubuntu recommended)
- GCC or another C++17-compatible compiler
- CMake 3.16+
- Python 3
- Git

Clone the repository:

```bash
git clone https://github.com/EyadAbrabu/NurseryLedger.git
cd NurseryLedger
```

Configure and build:

```bash
cmake -S . -B build
cmake --build build
```

Run the application:

```bash
./build/nurseryledger
```

## 2. Repository Structure

| Directory / File | Purpose |
|---|---|
| `include/` | C++ headers, interfaces, and data models |
| `src/` | Application implementation |
| `src/main.cpp` | Application entry point and startup/shutdown |
| `src/CLI.cpp` | Command parsing and user interaction |
| `src/Ledger.cpp` | Financial business rules |
| `src/Storage.cpp` | CSV persistence, snapshot saving, and loading |
| `src/Date.cpp` | Calendar date validation |
| `tests/` | C++ unit tests |
| `tests/integration/` | Python component tests |
| `docs/REQUIREMENTS.md` | Functional and non-functional requirements |
| `docs/ROADMAP.md` | Future functionality and known limitations |
| `docs/DEVELOPMENT_LOG.md` | Development decisions and debugging history |
| `CMakeLists.txt` | Build and C++ test configuration |

## 3. Understanding the Architecture

The application separates responsibilities into three main components.

**CLI** is responsible for user input, command parsing, and displaying results.

**Ledger** owns the financial rules, including registration, monthly billing, payments, credit allocation, and refunds.

**Storage** is responsible for serializing and restoring the application's data. It must not bypass Ledger validation when loading financial records.

The data models, including `Child`, `FeeObligation`, `Payment`, and `Refund`, represent the information used by these components.

When making changes, try to preserve these boundaries. Financial business rules should remain in the Ledger rather than being duplicated inside the CLI.

## 4. Running Tests

Testing is an important part of the development process, particularly because NurseryLedger handles financial calculations.

### C++ Unit Tests

```bash
ctest --test-dir build --output-on-failure
```

The existing six test suites cover:

- Date validation
- Child registration
- Monthly billing
- Payment recording
- Credit allocation
- Refund handling

### Python Component Tests

```bash
python3 tests/integration/test_cli.py
```

The seventeen Python scenarios execute the application externally and verify its behavior, including financial operations, persistence, invalid inputs, and simulated snapshot recovery failures.

### Complete Verification

```bash
cmake --build build && \
ctest --test-dir build --output-on-failure && \
python3 tests/integration/test_cli.py
```

Using `&&` ensures later tests are not executed against an outdated binary if compilation fails.

## 5. Financial Rules and Coding Conventions

When contributing to NurseryLedger, please follow these principles.

**Use integer cents for money.**

Financial amounts are represented using `std::int64_t`. Avoid introducing floating-point calculations for stored fees, payments, or refunds.

**Preserve the distinction between obligations and transactions.**

A monthly fee belongs to a billing period. A payment has an actual receipt date. These values should remain separate.

**Protect existing business rules.**

Do not allow duplicate monthly fee obligations or refunds exceeding eligible credit.

**Keep business logic independent of the CLI.**

Whenever possible, test new rules directly through the Ledger before adding command-line integration tests.

**Validate input before accepting financial operations.**

Malformed dates, monetary amounts, and stored records should not silently become valid transactions.

**Maintain C++17 compatibility.**

Avoid introducing language features requiring newer standards without documenting the reason and updating the build requirements.

## 6. Working with Persistent Data

NurseryLedger uses CSV records and versioned snapshots.

The `Storage` component writes datasets containing children, fee obligations, payments, and refunds. The `CURRENT` pointer identifies the committed snapshot.

Please take particular care when modifying persistence.

- Do not commit real or local financial CSV data to Git.
- Use temporary directories and fictional records when testing persistence.
- Preserve compatibility with existing saved records unless a migration is explicitly designed.
- Ensure incomplete or corrupted datasets are handled safely.
- Test both saving and loading when changing the storage format.

**Important:** The application currently saves during normal shutdown. Unexpected termination may result in unsaved changes being lost. Contributors should not assume that every successful command is immediately durable.

## 7. Adding or Changing Functionality

A suggested workflow is:

1. Check the existing requirements and roadmap.
2. Define the business rule or problem being addressed.
3. Implement the smallest appropriate change.
4. Add or update C++ unit tests when financial logic changes.
5. Add Python component tests when externally observable behavior changes.
6. Run the complete regression suite.
7. Update documentation when behavior or limitations change.

New functionality should not silently alter existing financial policies.

For example, supporting enrollment on arbitrary dates would require defining prorated billing rules rather than merely removing the enrollment-date restriction.

## 8. Git and Pull Requests

Use a dedicated branch for changes:

```bash
git checkout -b feature/short-description
```

Keep commits focused and descriptive.

Examples:

```text
feat(ledger): support new billing rule
fix(storage): validate incomplete datasets
test(storage): add persistence regression test
docs: clarify financial business rules
```

Before submitting a pull request:

- Confirm the application builds.
- Run all existing C++ and Python tests.
- Explain what changed and why.
- Describe any new tests.
- Document limitations or compatibility concerns.

For changes involving financial records, include at least one test demonstrating the relevant business rule or failure scenario.

## 9. Project Scope

NurseryLedger Version 1 focuses on nursery fee management.

Employee payroll, inventory, operational expenses, multi-user access, and advanced reporting are outside the current release.

These areas are documented in the [development roadmap](docs/ROADMAP.md).

Contributions should respect the current scope rather than introducing unrelated features without a clear design decision.

## 10. Final Note

One of the biggest lessons I learned while developing NurseryLedger was that writing code that works is only part of the job.

Understanding the business rules, testing unexpected situations, and making changes without damaging existing functionality are just as important.

I want the project to remain understandable to someone opening the repository for the first time.

Thank you for contributing!
