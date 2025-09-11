#ifndef CHTL_TEST_FRAMEWORK_H
#define CHTL_TEST_FRAMEWORK_H

#include <string>
#include <vector>
#include <functional>
#include <iostream>
#include <chrono>
#include <map>
#include <memory>

namespace CHTL {
namespace Test {

// 测试结果枚举
enum class TestResult {
    PASSED,
    FAILED,
    SKIPPED,
    ERROR
};

// 测试用例类
class TestCase {
public:
    std::string name;
    std::string description;
    std::function<void()> testFunction;
    TestResult result;
    std::string errorMessage;
    std::chrono::milliseconds duration;
    
    TestCase(const std::string& name, const std::string& description, std::function<void()> func)
        : name(name), description(description), testFunction(func), result(TestResult::PASSED) {}
    
    void run();
    std::string getResultString() const;
};

// 测试套件类
class TestSuite {
public:
    std::string name;
    std::vector<std::unique_ptr<TestCase>> testCases;
    std::map<std::string, int> statistics;
    
    TestSuite(const std::string& name) : name(name) {}
    
    void addTestCase(std::unique_ptr<TestCase> testCase);
    void runAll();
    void printResults() const;
    void printStatistics() const;
};

// 测试框架主类
class TestFramework {
private:
    std::vector<std::unique_ptr<TestSuite>> testSuites;
    std::map<std::string, int> globalStatistics;
    bool verboseMode;
    bool colorOutput;
    std::string outputFile;
    
    // 测试发现和过滤
    std::string testFilter;
    std::string suiteFilter;
    std::string tagFilter;
    std::map<std::string, std::vector<std::string>> testTags;
    
    // 测试配置
    int testTimeout;
    int maxRetries;
    bool parallelExecution;
    
    // 测试环境
    bool environmentSetup;
    std::map<std::string, std::string> environmentVariables;
    
public:
    TestFramework();
    ~TestFramework() = default;
    
    void addTestSuite(std::unique_ptr<TestSuite> suite);
    void runAllTests();
    void runTestSuite(const std::string& suiteName);
    void runTestCase(const std::string& suiteName, const std::string& testName);
    void printGlobalResults() const;
    void printGlobalStatistics() const;
    
    // 配置方法
    void setVerboseMode(bool verbose) { verboseMode = verbose; }
    void setColorOutput(bool color) { colorOutput = color; }
    void setOutputFile(const std::string& file) { outputFile = file; }
    
    // 断言宏
    static void assertTrue(bool condition, const std::string& message = "");
    static void assertFalse(bool condition, const std::string& message = "");
    static void assertEqual(const std::string& expected, const std::string& actual, const std::string& message = "");
    static void assertNotEqual(const std::string& expected, const std::string& actual, const std::string& message = "");
    static void assertNull(void* ptr, const std::string& message = "");
    static void assertNotNull(void* ptr, const std::string& message = "");
    static void assertThrows(std::function<void()> func, const std::string& message = "");
    static void assertNoThrow(std::function<void()> func, const std::string& message = "");
    
    // 高级断言
    static void assertContains(const std::string& haystack, const std::string& needle, const std::string& message = "");
    static void assertNotContains(const std::string& haystack, const std::string& needle, const std::string& message = "");
    static void assertStartsWith(const std::string& str, const std::string& prefix, const std::string& message = "");
    static void assertEndsWith(const std::string& str, const std::string& suffix, const std::string& message = "");
    static void assertEmpty(const std::string& str, const std::string& message = "");
    static void assertNotEmpty(const std::string& str, const std::string& message = "");
    static void assertGreaterThan(int expected, int actual, const std::string& message = "");
    static void assertLessThan(int expected, int actual, const std::string& message = "");
    static void assertGreaterThanOrEqual(int expected, int actual, const std::string& message = "");
    
    // 测试发现
    void discoverTests(const std::string& directory);
    void discoverTests(const std::string& directory, const std::string& pattern);
    std::vector<std::string> findTestFiles(const std::string& directory);
    
    // 测试报告
    void generateReport(const std::string& outputPath);
    void generateHTMLReport(const std::string& outputPath);
    void generateJSONReport(const std::string& outputPath);
    void generateXMLReport(const std::string& outputPath);
    
    // 测试统计
    std::map<std::string, int> getGlobalStatistics() const;
    std::string getTestSummary() const;
    double getSuccessRate() const;
    
    // 测试过滤
    void setTestFilter(const std::string& filter);
    void setSuiteFilter(const std::string& filter);
    void setTagFilter(const std::string& tag);
    
    // 测试标签
    void addTestTag(const std::string& testName, const std::string& tag);
    void removeTestTag(const std::string& testName, const std::string& tag);
    std::vector<std::string> getTestTags(const std::string& testName) const;
    
    // 测试配置
    void setTestTimeout(int timeoutMs);
    void setMaxRetries(int retries);
    void setParallelExecution(bool parallel);
    
    // 测试环境
    void setupTestEnvironment();
    void cleanupTestEnvironment();
    void resetTestEnvironment();
    static void assertLessThanOrEqual(int expected, int actual, const std::string& message = "");
    
    // 测试报告
    void generateReport() const;
    void exportResults(const std::string& filename) const;
    void printSummary() const;
    
    // 测试发现
    void discoverTests(const std::string& directory);
    void loadTestConfig(const std::string& configFile);
    void saveTestConfig(const std::string& configFile) const;
};

// 测试宏定义
#define CHTL_TEST_CASE(name, description) \
    CHTL::Test::TestCase name##_test(name, description, [&]()

#define CHTL_ASSERT_TRUE(condition) \
    CHTL::Test::TestFramework::assertTrue(condition, #condition)

#define CHTL_ASSERT_FALSE(condition) \
    CHTL::Test::TestFramework::assertFalse(condition, #condition)

#define CHTL_ASSERT_EQUAL(expected, actual) \
    CHTL::Test::TestFramework::assertEqual(expected, actual, #expected " == " #actual)

#define CHTL_ASSERT_NOT_EQUAL(expected, actual) \
    CHTL::Test::TestFramework::assertNotEqual(expected, actual, #expected " != " #actual)

#define CHTL_ASSERT_NULL(ptr) \
    CHTL::Test::TestFramework::assertNull(ptr, #ptr " is null")

#define CHTL_ASSERT_NOT_NULL(ptr) \
    CHTL::Test::TestFramework::assertNotNull(ptr, #ptr " is not null")

#define CHTL_ASSERT_THROWS(func) \
    CHTL::Test::TestFramework::assertThrows(func, #func " should throw")

#define CHTL_ASSERT_NO_THROW(func) \
    CHTL::Test::TestFramework::assertNoThrow(func, #func " should not throw")

} // namespace Test
} // namespace CHTL

#endif // CHTL_TEST_FRAMEWORK_H