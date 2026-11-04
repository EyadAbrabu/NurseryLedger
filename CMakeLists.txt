cmake_minimum_required(VERSION 3.16)

project(NurseryLedger LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(nurseryledger
    src/main.cpp
    src/Date.cpp
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
