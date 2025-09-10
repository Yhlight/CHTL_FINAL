#include "TestFramework.h"
#include <stdexcept>
#include <sstream>
#include <iomanip>

namespace CHTL {
namespace Test {

// TestCase 实现
void TestCase::run() {
    auto start = std::chrono::high_resolution_clock::now();
    
    try {
        testFunction();
        result = TestResult::PASSED;
    } catch (const std::exception& e) {
        result = TestResult::ERROR;
        errorMessage = e.what();
    } catch (...) {
        result = TestResult::ERROR;
        errorMessage = "Unknown error occurred";
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
}

std::string TestCase::getResultString() const {
    switch (result) {
        case TestResult::PASSED:
            return "PASSED";
        case TestResult::FAILED:
            return "FAILED";
        case TestResult::SKIPPED:
            return "SKIPPED";
        case TestResult::ERROR:
            return "ERROR";
        default:
            return "UNKNOWN";
    }
}

// TestSuite 实现
void TestSuite::addTestCase(std::unique_ptr<TestCase> testCase) {
    testCases.push_back(std::move(testCase));
}

void TestSuite::runAll() {
    std::cout << "运行测试套件: " << name << std::endl;
    std::cout << "===========================================" << std::endl;
    
    statistics["total"] = testCases.size();
    statistics["passed"] = 0;
    statistics["failed"] = 0;
    statistics["skipped"] = 0;
    statistics["error"] = 0;
    
    for (auto& testCase : testCases) {
        std::cout << "运行测试: " << testCase->name << " - " << testCase->description << std::endl;
        testCase->run();
        
        std::cout << "结果: " << testCase->getResultString();
        if (testCase->result == TestResult::ERROR && !testCase->errorMessage.empty()) {
            std::cout << " - " << testCase->errorMessage;
        }
        std::cout << " (耗时: " << testCase->duration.count() << "ms)" << std::endl;
        
        statistics[testCase->getResultString()]++;
    }
    
    std::cout << std::endl;
}

void TestSuite::printResults() const {
    std::cout << "测试套件结果: " << name << std::endl;
    std::cout << "===========================================" << std::endl;
    
    for (const auto& testCase : testCases) {
        std::cout << testCase->name << ": " << testCase->getResultString();
        if (testCase->result == TestResult::ERROR && !testCase->errorMessage.empty()) {
            std::cout << " - " << testCase->errorMessage;
        }
        std::cout << std::endl;
    }
    
    std::cout << std::endl;
}

void TestSuite::printStatistics() const {
    std::cout << "测试套件统计: " << name << std::endl;
    std::cout << "===========================================" << std::endl;
    std::cout << "总计: " << statistics.at("total") << std::endl;
    std::cout << "通过: " << statistics.at("passed") << std::endl;
    std::cout << "失败: " << statistics.at("failed") << std::endl;
    std::cout << "跳过: " << statistics.at("skipped") << std::endl;
    std::cout << "错误: " << statistics.at("error") << std::endl;
    std::cout << std::endl;
}

// TestFramework 实现
TestFramework::TestFramework() {
    globalStatistics["total"] = 0;
    globalStatistics["passed"] = 0;
    globalStatistics["failed"] = 0;
    globalStatistics["skipped"] = 0;
    globalStatistics["error"] = 0;
}

void TestFramework::addTestSuite(std::unique_ptr<TestSuite> suite) {
    testSuites.push_back(std::move(suite));
}

void TestFramework::runAllTests() {
    std::cout << "开始运行所有测试" << std::endl;
    std::cout << "===========================================" << std::endl;
    
    for (auto& suite : testSuites) {
        suite->runAll();
        
        // 更新全局统计
        for (const auto& stat : suite->statistics) {
            globalStatistics[stat.first] += stat.second;
        }
    }
    
    printGlobalResults();
    printGlobalStatistics();
}

void TestFramework::printGlobalResults() const {
    std::cout << "全局测试结果" << std::endl;
    std::cout << "===========================================" << std::endl;
    
    for (const auto& suite : testSuites) {
        suite->printResults();
    }
}

void TestFramework::printGlobalStatistics() const {
    std::cout << "全局测试统计" << std::endl;
    std::cout << "===========================================" << std::endl;
    std::cout << "总计: " << globalStatistics.at("total") << std::endl;
    std::cout << "通过: " << globalStatistics.at("passed") << std::endl;
    std::cout << "失败: " << globalStatistics.at("failed") << std::endl;
    std::cout << "跳过: " << globalStatistics.at("skipped") << std::endl;
    std::cout << "错误: " << globalStatistics.at("error") << std::endl;
    
    if (globalStatistics.at("total") > 0) {
        double passRate = (double)globalStatistics.at("passed") / globalStatistics.at("total") * 100.0;
        std::cout << "通过率: " << std::fixed << std::setprecision(2) << passRate << "%" << std::endl;
    }
    
    std::cout << std::endl;
}

// 断言方法实现
void TestFramework::assertTrue(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error("断言失败: " + message);
    }
}

void TestFramework::assertFalse(bool condition, const std::string& message) {
    if (condition) {
        throw std::runtime_error("断言失败: " + message);
    }
}

void TestFramework::assertEqual(const std::string& expected, const std::string& actual, const std::string& message) {
    if (expected != actual) {
        std::ostringstream oss;
        oss << "断言失败: " << message << " (期望: '" << expected << "', 实际: '" << actual << "')";
        throw std::runtime_error(oss.str());
    }
}

void TestFramework::assertNotEqual(const std::string& expected, const std::string& actual, const std::string& message) {
    if (expected == actual) {
        std::ostringstream oss;
        oss << "断言失败: " << message << " (值不应该相等: '" << expected << "')";
        throw std::runtime_error(oss.str());
    }
}

void TestFramework::assertNull(void* ptr, const std::string& message) {
    if (ptr != nullptr) {
        throw std::runtime_error("断言失败: " + message);
    }
}

void TestFramework::assertNotNull(void* ptr, const std::string& message) {
    if (ptr == nullptr) {
        throw std::runtime_error("断言失败: " + message);
    }
}

void TestFramework::assertThrows(std::function<void()> func, const std::string& message) {
    bool threw = false;
    try {
        func();
    } catch (...) {
        threw = true;
    }
    
    if (!threw) {
        throw std::runtime_error("断言失败: " + message);
    }
}

void TestFramework::assertNoThrow(std::function<void()> func, const std::string& message) {
    try {
        func();
    } catch (const std::exception& e) {
        throw std::runtime_error("断言失败: " + message + " (异常: " + e.what() + ")");
    } catch (...) {
        throw std::runtime_error("断言失败: " + message + " (未知异常)");
    }
}

} // namespace Test
} // namespace CHTL