#ifndef CHTL_TEST_H
#define CHTL_TEST_H

#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <map>
#include <sstream>

namespace CHTL_TEST {

// Simple assertion macros for testing
#define ASSERT_TRUE(condition) \
    if (!(condition)) { \
        std::cerr << "\n  Assertion failed: " #condition " is not true at " << __FILE__ << ":" << __LINE__; \
        return false; \
    }

#define ASSERT_FALSE(condition) \
    if (condition) { \
        std::cerr << "\n  Assertion failed: " #condition " is not false at " << __FILE__ << ":" << __LINE__; \
        return false; \
    }

#define ASSERT_EQUAL(expected, actual) \
    if (!((expected) == (actual))) { \
        std::stringstream ss_expected; \
        std::stringstream ss_actual; \
        ss_expected << (expected); \
        ss_actual << (actual); \
        std::cerr << "\n  Assertion failed at " << __FILE__ << ":" << __LINE__ \
                  << "\n  Expected: " << ss_expected.str() \
                  << "\n  Actual  : " << ss_actual.str(); \
        return false; \
    }

// A singleton class to manage and run all registered tests.
class TestRunner {
public:
    using TestCase = std::function<bool()>;

    static TestRunner& getInstance() {
        static TestRunner instance;
        return instance;
    }

    void registerTest(const std::string& testName, TestCase testCase) {
        m_tests[testName] = testCase;
    }

    // Runs all registered tests and returns the number of failures.
    int runAll() {
        int failed_count = 0;
        std::cout << "Running " << m_tests.size() << " tests...\n";
        std::cout << "----------------------------------------\n";
        for (const auto& pair : m_tests) {
            std::cout << "[ RUN      ] " << pair.first;
            bool result = pair.second();
            if (result) {
                std::cout << "\n[       OK ] " << pair.first << std::endl;
            } else {
                std::cout << "\n[   FAILED ] " << pair.first << std::endl;
                failed_count++;
            }
        }
        std::cout << "----------------------------------------\n";
        if (failed_count == 0) {
            std::cout << "All tests passed.\n";
        } else {
            std::cerr << failed_count << " test(s) failed.\n";
        }
        return failed_count;
    }

private:
    TestRunner() = default;
    TestRunner(const TestRunner&) = delete;
    TestRunner& operator=(const TestRunner&) = delete;
    std::map<std::string, TestCase> m_tests;
};

// A helper class that uses RAII to register a test case upon construction.
class TestRegisterer {
public:
    TestRegisterer(const std::string& testName, TestRunner::TestCase testCase) {
        TestRunner::getInstance().registerTest(testName, testCase);
    }
};

// A macro to simplify the definition and registration of a test case.
#define TEST_CASE(name) \
    bool test_##name(); \
    static CHTL_TEST::TestRegisterer registerer_##name(#name, test_##name); \
    bool test_##name()

} // namespace CHTL_TEST

#endif // CHTL_TEST_H
