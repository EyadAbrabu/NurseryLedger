# NurseryLedger — Development Log

## Problem 001: Calendar Date Validation

**Date:** 2026-11-04  
**Component:** Date  
**Status:** Solution planned; implementation and verification pending

### Problem

The initial `Date::isValid()` implementation only checked whether the month was between 1 and 12 and the day was between 1 and 31.

This allowed impossible dates, such as February 31, to be considered valid.

### Investigation

I considered using an existing calendar library, similar to Python's `datetime` module.

I learned that C++20 provides calendar-aware date types through `<chrono>`, but the project currently targets C++17.

### Decision

I decided to implement a small calendar validator using C++17.

The validator will account for the number of days in each month and Gregorian leap-year rules.

### Trade-offs

This avoids adding dependencies and makes the validation logic straightforward to unit test. However, implementing date validation manually introduces the risk of missing edge cases.

### Verification Plan

- Reject February 31.
- Accept February 29 in leap years.
- Reject February 29 in non-leap years.
- Reject invalid months and days.
- Verify century exceptions, including 1900 and 2000.

### Outcome

Pending implementation and automated tests.
## November 5, 2026 — Refund Handling and Testing

### Implementation
- Introduced a Refund data structure and integrated refund handling into the Ledger class.
- Implemented refunds against unused credit originating from a specific billing period.
- Preserved original payment records to maintain transaction history.
- Expanded regression tests for partial refunds, exhausted credit, invalid transactions, and separation between children's balances.

### Problems encountered and solutions

**1. Compilation errors caused by duplicated code**

While modifying the credit-allocation algorithm, an obsolete section of code remained outside the function and namespace boundaries.

The compiler reported errors involving unexpected `for` loops and undefined variables.

We inspected the numbered source file, identified the duplicate implementation, removed it, and rebuilt successfully.

**2. Incorrect refund test expectations**

A test attempted to refund credit that had already been applied to a subsequent month's fee.

The implementation rejected the refund, which was consistent with the business rule that only unused credit can be refunded.

We redesigned the test to refund credit before generating the next month's obligation.

**3. Loss of test coverage during debugging**

Removing the incorrect assertions initially removed coverage for multiple partial refunds.

We restored that coverage using independent test scenarios and added checks for account isolation and preservation of payment records.

### Verification

All six CTest suites passed after the expanded refund tests.

### Known limitation

The current allocation engine does not fully reconstruct financial balances chronologically from transaction dates. This remains an area for future improvement.
