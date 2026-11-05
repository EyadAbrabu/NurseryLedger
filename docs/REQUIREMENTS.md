# NurseryLedger — V1 Requirements

## 1. Project Overview

NurseryLedger is a Linux-based C++17 command-line application designed to help nursery administrators manage monthly fee obligations, record payments, and monitor outstanding balances.

The project is inspired by financial and administrative challenges observed in a family-operated nursery.

## 2. Problem Statement

Nursery administrators may face difficulties when:

- Tracking which children have paid their monthly fees.
- Recording cash payments separately from bank transfers.
- Managing partial payments and advance payments.
- Identifying overdue fees.
- Calculating total outstanding balances.
- Maintaining consistent records between months.

NurseryLedger aims to address these problems through a focused, locally operated financial record-keeping application.

## 3. Functional Requirements

**FR-01 — Child Registration**

The system shall register a child using a unique ID, name, enrollment date, and monthly fee.

**FR-02 — Enrollment Validation**

The system shall accept enrollment dates only on the 1st or 15th day of a month.

**FR-03 — Monthly Billing**

The system shall charge the full monthly fee beginning on the enrollment date.

Subsequent monthly fees shall become due on the same calendar day as enrollment.

**FR-04 — Payment Recording**

The system shall record payments containing:

- Child ID
- Payment amount
- Payment date
- Payment method: cash or bank transfer
- Billing period to which the payment is allocated, where applicable

**FR-05 — Partial Payments**

The system shall support multiple payments toward a monthly fee obligation.

**FR-06 — Advance Payments and Credit**

When a payment exceeds the current outstanding balance, the system shall retain the excess as credit and apply it to future fee obligations.

Credit application shall not create an additional cash or bank receipt.

**FR-07 — Outstanding Balance Calculation**

The system shall calculate the remaining unpaid balance for each child's billing period after payments and applicable credits.

**FR-08 — Monthly Billing Summary**

The system shall display total fees due, amounts settled, and outstanding balances for a selected billing month.

**FR-09 — Payment Method Reporting**

The system shall separately report cash and bank-transfer receipts using their actual payment dates.

**FR-10 — Persistent Storage**

The system shall save child, payment, and credit information locally and restore it between application executions.

**FR-11 — Input Validation**

The system shall reject invalid dates, negative or zero payment amounts, unknown child IDs, and unsupported payment methods.

**FR-12 — Payment History**

The system shall allow an administrator to view recorded payment transactions for a child.

**FR-13 — Prevention of Duplicate Monthly Billing**

The system shall ensure that each child has no more than one fee obligation per billing month and year.

- Duplicate billing attempts shall not create additional obligations.
- Existing obligations shall remain unchanged.
- Billing for subsequent months shall remain possible.
## 4. Non-Functional Requirements

**NFR-01 — Platform Compatibility**

The application shall compile and run on Linux using C++17.

**NFR-02 — Financial Accuracy**

Monetary amounts shall be represented using integer minor units to avoid floating-point rounding errors.

**NFR-03 — Data Integrity**

The application shall detect storage errors and avoid silently accepting incomplete or corrupted records.

**NFR-04 — Usability**

The command-line interface shall provide understandable commands, clear output, and actionable error messages.

**NFR-05 — Maintainability**

Business logic, command-line interaction, and persistent storage shall be organized into separate components.

**NFR-06 — Unit Testing**

Core billing, validation, payment, and credit calculations shall be covered by automated C++ unit tests.

**NFR-07 — Component Testing**

Python component tests shall execute the compiled application and verify externally observable behavior.

**NFR-08 — Reproducibility**

The repository shall include instructions for building, running, and testing the application.

**NFR-09 — Privacy**

The public repository and sample data shall not contain real children's personal or financial information.

## 5. Business Rules

- BR-01: Enrollment is permitted only on the 1st or 15th.
- BR-02: The full monthly fee is charged at enrollment.
- BR-03: Monthly fees recur on the child's enrollment day.
- BR-04: Partial payments are permitted.
- BR-05: Excess payments become credit for future fees.
- BR-06: Credit application does not create a second payment receipt.
- BR-07: Billing reports and receipt reports use different date concepts.
- BR-08: Financial amounts are stored in integer minor units.

## 6. Initial Acceptance Scenarios

**Scenario A — Enrollment**

Given a valid child record with an enrollment date of November 15, the system accepts registration and establishes a full fee obligation due on November 15.

**Scenario B — Invalid Enrollment**

Given an enrollment date of November 9, the system rejects registration with a clear error.

**Scenario C — Partial Payment**

Given a €300 fee and a €100 payment, the remaining outstanding amount is €200.

**Scenario D — Advance Payment**

Given a €300 fee and a €350 payment, the current fee is fully settled and €50 remains available as credit for future obligations.

**Scenario E — Payment Reporting**

Given a bank transfer received on December 2 for a November fee, the payment appears in December's receipt totals while reducing the outstanding November fee balance.

**Scenario F — Persistence**

Given a successfully recorded child and payment, the records remain available after the application is closed and reopened.

## 7. V1 Exclusions

The following features are not included in V1:

- Employee wages and loans
- Food, diaper, and cleaning inventory
- Operational expense budgets
- Net-profit accounting
- Graphical user interface
- Online banking integration
- Multi-user authentication

## 8. Known Design Questions

The following implementation details will be resolved during architecture and development:

- How to identify and order fee obligations across months.
- How advance credit is allocated to future billing periods.
- How to handle payment dates before enrollment.
- How to recover from interrupted storage operations.
- How to handle invalid or manually modified storage files.
