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
    
public:
    TestFramework();
    ~TestFramework() = default;
    
    void addTestSuite(std::unique_ptr<TestSuite> suite);
    void runAllTests();
    void printGlobalResults() const;
    void printGlobalStatistics() const;
    
    // 断言宏
    static void assertTrue(bool condition, const std::string& message = "");
    static void assertFalse(bool condition, const std::string& message = "");
    static void assertEqual(const std::string& expected, const std::string& actual, const std::string& message = "");
    static void assertNotEqual(const std::string& expected, const std::string& actual, const std::string& message = "");
    static void assertNull(void* ptr, const std::string& message = "");
    static void assertNotNull(void* ptr, const std::string& message = "");
    static void assertThrows(std::function<void()> func, const std::string& message = "");
    static void assertNoThrow(std::function<void()> func, const std::string& message = "");
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