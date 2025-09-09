#pragma once

#include <string>
#include <vector>
#include <functional>
#include <iostream>
#include <chrono>

namespace CHTL {

/**
 * @brief 测试结果
 */
struct TestResult {
    std::string name;
    bool passed;
    std::string message;
    double duration;
    
    TestResult(const std::string& n, bool p, const std::string& m = "", double d = 0.0)
        : name(n), passed(p), message(m), duration(d) {}
};

/**
 * @brief 测试套件
 */
class CHTLTestSuite {
public:
    CHTLTestSuite(const std::string& name);
    ~CHTLTestSuite();
    
    // 添加测试
    void addTest(const std::string& name, std::function<bool()> test);
    
    // 运行测试
    void run();
    void runTest(const std::string& name);
    
    // 获取结果
    const std::vector<TestResult>& getResults() const;
    size_t getPassedCount() const;
    size_t getFailedCount() const;
    double getTotalDuration() const;
    
    // 打印结果
    void printResults() const;
    void printSummary() const;

private:
    std::string m_name;
    std::vector<std::pair<std::string, std::function<bool()>>> m_tests;
    std::vector<TestResult> m_results;
    
    // 辅助方法
    void printHeader() const;
    void printTestResult(const TestResult& result) const;
};

/**
 * @brief 测试断言
 */
class TestAssert {
public:
    static bool assertTrue(bool condition, const std::string& message = "");
    static bool assertFalse(bool condition, const std::string& message = "");
    static bool assertEquals(const std::string& expected, const std::string& actual, const std::string& message = "");
    static bool assertNotEquals(const std::string& expected, const std::string& actual, const std::string& message = "");
    static bool assertContains(const std::string& container, const std::string& content, const std::string& message = "");
    static bool assertNotEmpty(const std::string& str, const std::string& message = "");
    static bool assertThrows(std::function<void()> func, const std::string& message = "");
};

/**
 * @brief 测试工具
 */
class TestUtils {
public:
    // 文件操作
    static std::string readTestFile(const std::string& filename);
    static bool writeTestFile(const std::string& filename, const std::string& content);
    static bool deleteTestFile(const std::string& filename);
    
    // 字符串操作
    static std::string trim(const std::string& str);
    static std::vector<std::string> split(const std::string& str, char delimiter);
    static bool startsWith(const std::string& str, const std::string& prefix);
    static bool endsWith(const std::string& str, const std::string& suffix);
    
    // 时间操作
    static double getCurrentTime();
};

} // namespace CHTL