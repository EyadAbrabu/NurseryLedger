cmake_minimum_required(VERSION 3.16)

project(NurseryLedger LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(nurseryledger
    src/main.cpp
    src/Date.cpp
    src/Ledger.cpp
)

target_include_directories(nurseryledger PRIVATE include)

target_compile_options(nurseryledger PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)
enable_testing()




add_executable(test_date
     tests/test_date.cpp
     src/Date.cpp
)

target_include_directories(test_date PRIVATE include)

add_test(NAME DateValidation COMMAND test_date)




add_executable(test_ledger
    tests/test_ledger.cpp
    src/Ledger.cpp
    src/Date.cpp
)

target_include_directories(test_ledger PRIVATE include)

add_test(NAME LedgerRegistration COMMAND test_ledger)



add_executable(test_billing
    tests/test_billing.cpp
    src/Ledger.cpp
    src/Date.cpp
)

target_include_directories(test_billing PRIVATE include)

add_test(NAME MonthlyBilling COMMAND test_billing)



add_executable(test_payment
    tests/test_payment.cpp
    src/Ledger.cpp
    src/Date.cpp
)

target_include_directories(test_payment PRIVATE include)

add_test(NAME PaymentRecording COMMAND test_payment)

