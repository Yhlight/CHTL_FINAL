#include "Test/CHTLTest.h"
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <cstdio>

namespace CHTL {

CHTLTestSuite::CHTLTestSuite(const std::string& name) : m_name(name) {
}

CHTLTestSuite::~CHTLTestSuite() = default;

void CHTLTestSuite::addTest(const std::string& name, std::function<bool()> test) {
    m_tests.push_back({name, test});
}

void CHTLTestSuite::run() {
    printHeader();
    
    m_results.clear();
    
    for (const auto& test : m_tests) {
        runTest(test.first);
    }
    
    printSummary();
}

void CHTLTestSuite::runTest(const std::string& name) {
    auto it = std::find_if(m_tests.begin(), m_tests.end(),
                          [&name](const auto& test) { return test.first == name; });
    
    if (it == m_tests.end()) {
        m_results.push_back(TestResult(name, false, "Test not found"));
        return;
    }
    
    double startTime = TestUtils::getCurrentTime();
    bool passed = false;
    std::string message;
    
    try {
        passed = it->second();
        if (passed) {
            message = "PASSED";
        } else {
            message = "FAILED";
        }
    } catch (const std::exception& e) {
        passed = false;
        message = "EXCEPTION: " + std::string(e.what());
    } catch (...) {
        passed = false;
        message = "UNKNOWN EXCEPTION";
    }
    
    double endTime = TestUtils::getCurrentTime();
    double duration = endTime - startTime;
    
    m_results.push_back(TestResult(name, passed, message, duration));
    printTestResult(m_results.back());
}

const std::vector<TestResult>& CHTLTestSuite::getResults() const {
    return m_results;
}

size_t CHTLTestSuite::getPassedCount() const {
    return std::count_if(m_results.begin(), m_results.end(),
                        [](const TestResult& result) { return result.passed; });
}

size_t CHTLTestSuite::getFailedCount() const {
    return m_results.size() - getPassedCount();
}

double CHTLTestSuite::getTotalDuration() const {
    double total = 0.0;
    for (const auto& result : m_results) {
        total += result.duration;
    }
    return total;
}

void CHTLTestSuite::printResults() const {
    printHeader();
    
    for (const auto& result : m_results) {
        printTestResult(result);
    }
    
    printSummary();
}

void CHTLTestSuite::printSummary() const {
    std::cout << "\n" << std::string(50, '=') << std::endl;
    std::cout << "Test Suite: " << m_name << std::endl;
    std::cout << "Total Tests: " << m_results.size() << std::endl;
    std::cout << "Passed: " << getPassedCount() << std::endl;
    std::cout << "Failed: " << getFailedCount() << std::endl;
    std::cout << "Duration: " << std::fixed << std::setprecision(3) << getTotalDuration() << "s" << std::endl;
    std::cout << std::string(50, '=') << std::endl;
}

void CHTLTestSuite::printHeader() const {
    std::cout << "\nRunning Test Suite: " << m_name << std::endl;
    std::cout << std::string(50, '-') << std::endl;
}

void CHTLTestSuite::printTestResult(const TestResult& result) const {
    std::cout << std::left << std::setw(30) << result.name;
    std::cout << std::right << std::setw(10) << (result.passed ? "PASS" : "FAIL");
    std::cout << std::right << std::setw(10) << std::fixed << std::setprecision(3) << result.duration << "s";
    
    if (!result.message.empty() && result.message != "PASSED" && result.message != "FAILED") {
        std::cout << " - " << result.message;
    }
    
    std::cout << std::endl;
}

// TestAssert 实现
bool TestAssert::assertTrue(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error("Assertion failed: " + message);
    }
    return true;
}

bool TestAssert::assertFalse(bool condition, const std::string& message) {
    if (condition) {
        throw std::runtime_error("Assertion failed: " + message);
    }
    return true;
}

bool TestAssert::assertEquals(const std::string& expected, const std::string& actual, const std::string& message) {
    if (expected != actual) {
        throw std::runtime_error("Assertion failed: " + message + 
                                " (expected: '" + expected + "', actual: '" + actual + "')");
    }
    return true;
}

bool TestAssert::assertNotEquals(const std::string& expected, const std::string& actual, const std::string& message) {
    if (expected == actual) {
        throw std::runtime_error("Assertion failed: " + message + 
                                " (expected not equal, but both are: '" + expected + "')");
    }
    return true;
}

bool TestAssert::assertContains(const std::string& container, const std::string& content, const std::string& message) {
    if (container.find(content) == std::string::npos) {
        throw std::runtime_error("Assertion failed: " + message + 
                                " (container does not contain: '" + content + "')");
    }
    return true;
}

bool TestAssert::assertNotEmpty(const std::string& str, const std::string& message) {
    if (str.empty()) {
        throw std::runtime_error("Assertion failed: " + message + " (string is empty)");
    }
    return true;
}

bool TestAssert::assertThrows(std::function<void()> func, const std::string& message) {
    try {
        func();
        throw std::runtime_error("Assertion failed: " + message + " (expected exception, but none was thrown)");
    } catch (...) {
        return true;
    }
}

// TestUtils 实现
std::string TestUtils::readTestFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open test file: " + filename);
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    file.close();
    
    return content;
}

bool TestUtils::writeTestFile(const std::string& filename, const std::string& content) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << content;
    file.close();
    
    return true;
}

bool TestUtils::deleteTestFile(const std::string& filename) {
    return std::remove(filename.c_str()) == 0;
}

std::string TestUtils::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) {
        return "";
    }
    
    size_t end = str.find_last_not_of(" \t\n\r");
    return str.substr(start, end - start + 1);
}

std::vector<std::string> TestUtils::split(const std::string& str, char delimiter) {
    std::vector<std::string> result;
    std::stringstream ss(str);
    std::string item;
    
    while (std::getline(ss, item, delimiter)) {
        result.push_back(item);
    }
    
    return result;
}

bool TestUtils::startsWith(const std::string& str, const std::string& prefix) {
    return str.length() >= prefix.length() && 
           str.substr(0, prefix.length()) == prefix;
}

bool TestUtils::endsWith(const std::string& str, const std::string& suffix) {
    return str.length() >= suffix.length() && 
           str.substr(str.length() - suffix.length()) == suffix;
}

double TestUtils::getCurrentTime() {
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = now.time_since_epoch();
    return std::chrono::duration<double>(duration).count();
}

} // namespace CHTL