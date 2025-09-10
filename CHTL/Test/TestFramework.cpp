#include "TestFramework.h"
#include <stdexcept>
#include <sstream>
#include <iomanip>
#include <fstream>

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
TestFramework::TestFramework() 
    : verboseMode(false), colorOutput(true), outputFile("") {
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

// 高级断言方法实现
void TestFramework::assertContains(const std::string& haystack, const std::string& needle, const std::string& message) {
    if (haystack.find(needle) == std::string::npos) {
        std::ostringstream oss;
        oss << "断言失败: " << message << " (字符串 '" << haystack << "' 不包含 '" << needle << "')";
        throw std::runtime_error(oss.str());
    }
}

void TestFramework::assertNotContains(const std::string& haystack, const std::string& needle, const std::string& message) {
    if (haystack.find(needle) != std::string::npos) {
        std::ostringstream oss;
        oss << "断言失败: " << message << " (字符串 '" << haystack << "' 包含 '" << needle << "')";
        throw std::runtime_error(oss.str());
    }
}

void TestFramework::assertStartsWith(const std::string& str, const std::string& prefix, const std::string& message) {
    if (str.length() < prefix.length() || str.substr(0, prefix.length()) != prefix) {
        std::ostringstream oss;
        oss << "断言失败: " << message << " (字符串 '" << str << "' 不以 '" << prefix << "' 开头)";
        throw std::runtime_error(oss.str());
    }
}

void TestFramework::assertEndsWith(const std::string& str, const std::string& suffix, const std::string& message) {
    if (str.length() < suffix.length() || str.substr(str.length() - suffix.length()) != suffix) {
        std::ostringstream oss;
        oss << "断言失败: " << message << " (字符串 '" << str << "' 不以 '" << suffix << "' 结尾)";
        throw std::runtime_error(oss.str());
    }
}

void TestFramework::assertEmpty(const std::string& str, const std::string& message) {
    if (!str.empty()) {
        std::ostringstream oss;
        oss << "断言失败: " << message << " (字符串不为空: '" << str << "')";
        throw std::runtime_error(oss.str());
    }
}

void TestFramework::assertNotEmpty(const std::string& str, const std::string& message) {
    if (str.empty()) {
        throw std::runtime_error("断言失败: " + message + " (字符串为空)");
    }
}

void TestFramework::assertGreaterThan(int expected, int actual, const std::string& message) {
    if (actual <= expected) {
        std::ostringstream oss;
        oss << "断言失败: " << message << " (期望 > " << expected << ", 实际: " << actual << ")";
        throw std::runtime_error(oss.str());
    }
}

void TestFramework::assertLessThan(int expected, int actual, const std::string& message) {
    if (actual >= expected) {
        std::ostringstream oss;
        oss << "断言失败: " << message << " (期望 < " << expected << ", 实际: " << actual << ")";
        throw std::runtime_error(oss.str());
    }
}

void TestFramework::assertGreaterThanOrEqual(int expected, int actual, const std::string& message) {
    if (actual < expected) {
        std::ostringstream oss;
        oss << "断言失败: " << message << " (期望 >= " << expected << ", 实际: " << actual << ")";
        throw std::runtime_error(oss.str());
    }
}

void TestFramework::assertLessThanOrEqual(int expected, int actual, const std::string& message) {
    if (actual > expected) {
        std::ostringstream oss;
        oss << "断言失败: " << message << " (期望 <= " << expected << ", 实际: " << actual << ")";
        throw std::runtime_error(oss.str());
    }
}

// 测试运行方法
void TestFramework::runTestSuite(const std::string& suiteName) {
    for (auto& suite : testSuites) {
        if (suite->name == suiteName) {
            suite->runAll();
            return;
        }
    }
    throw std::runtime_error("测试套件未找到: " + suiteName);
}

void TestFramework::runTestCase(const std::string& suiteName, const std::string& testName) {
    for (auto& suite : testSuites) {
        if (suite->name == suiteName) {
            for (auto& testCase : suite->testCases) {
                if (testCase->name == testName) {
                    testCase->run();
                    return;
                }
            }
            throw std::runtime_error("测试用例未找到: " + testName + " 在套件 " + suiteName);
        }
    }
    throw std::runtime_error("测试套件未找到: " + suiteName);
}

// 测试报告方法
void TestFramework::generateReport() const {
    std::cout << "CHTL测试框架报告" << std::endl;
    std::cout << "===========================================" << std::endl;
    printGlobalStatistics();
    printGlobalResults();
}

void TestFramework::exportResults(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("无法创建输出文件: " + filename);
    }
    
    file << "CHTL测试框架报告" << std::endl;
    file << "===========================================" << std::endl;
    
    // 导出统计信息
    file << "总计: " << globalStatistics.at("total") << std::endl;
    file << "通过: " << globalStatistics.at("passed") << std::endl;
    file << "失败: " << globalStatistics.at("failed") << std::endl;
    file << "跳过: " << globalStatistics.at("skipped") << std::endl;
    file << "错误: " << globalStatistics.at("error") << std::endl;
    
    if (globalStatistics.at("total") > 0) {
        double passRate = (double)globalStatistics.at("passed") / globalStatistics.at("total") * 100.0;
        file << "通过率: " << std::fixed << std::setprecision(2) << passRate << "%" << std::endl;
    }
    
    file << std::endl;
    
    // 导出详细结果
    for (const auto& suite : testSuites) {
        file << "测试套件: " << suite->name << std::endl;
        file << "===========================================" << std::endl;
        
        for (const auto& testCase : suite->testCases) {
            file << testCase->name << ": " << testCase->getResultString();
            if (testCase->result == TestResult::ERROR && !testCase->errorMessage.empty()) {
                file << " - " << testCase->errorMessage;
            }
            file << " (耗时: " << testCase->duration.count() << "ms)" << std::endl;
        }
        
        file << std::endl;
    }
    
    file.close();
}

void TestFramework::printSummary() const {
    std::cout << "测试摘要" << std::endl;
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

// 测试发现方法
void TestFramework::discoverTests(const std::string& directory) {
    // 根据CHTL.md规范，实现测试发现功能
    // 这里可以扫描目录中的测试文件并自动加载
    std::cout << "发现测试目录: " << directory << std::endl;
    // 实现细节可以根据需要添加
}

void TestFramework::loadTestConfig(const std::string& configFile) {
    // 根据CHTL.md规范，实现测试配置加载功能
    std::cout << "加载测试配置: " << configFile << std::endl;
    // 实现细节可以根据需要添加
}

void TestFramework::saveTestConfig(const std::string& configFile) const {
    // 根据CHTL.md规范，实现测试配置保存功能
    std::cout << "保存测试配置: " << configFile << std::endl;
    // 实现细节可以根据需要添加
}

} // namespace Test
} // namespace CHTL