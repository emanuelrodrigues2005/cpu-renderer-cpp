#pragma once

#include <iostream>
#include <string>

namespace test {

inline int& failureCount() {
    static int count = 0;
    return count;
}

inline void reportFailure(const char* file, int line, const std::string& message) {
    ++failureCount();
    std::cerr << file << ":" << line << ": FALHOU: " << message << "\n";
}

}  // namespace test

#define CHECK(condition)                                                        \
    do {                                                                        \
        if (!(condition)) {                                                     \
            test::reportFailure(__FILE__, __LINE__, #condition);                \
        }                                                                       \
    } while (false)

#define CHECK_EQ(actual, expected)                                              \
    do {                                                                        \
        const auto& actualValue = (actual);                                     \
        const auto& expectedValue = (expected);                                 \
        if (!(actualValue == expectedValue)) {                                  \
            test::reportFailure(__FILE__, __LINE__,                             \
                                std::string(#actual) + " == " + #expected);     \
        }                                                                       \
    } while (false)

#define CHECK_NEAR(actual, expected, tolerance)                                 \
    do {                                                                        \
        const auto actualValue = (actual);                                      \
        const auto expectedValue = (expected);                                  \
        const auto difference = actualValue - expectedValue;                    \
        if (!(difference <= (tolerance) && difference >= -(tolerance))) {       \
            test::reportFailure(__FILE__, __LINE__,                             \
                                std::string(#actual) + " ~= " + #expected);     \
        }                                                                       \
    } while (false)
