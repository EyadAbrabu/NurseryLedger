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
