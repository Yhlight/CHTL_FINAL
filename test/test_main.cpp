#include <iostream>
#include <string>
#include <vector>

// 简单的测试框架
class TestFramework {
public:
    static int runTests() {
        int passed = 0;
        int total = 0;
        
        std::cout << "Running CHTL Tests...\n\n";
        
        // 运行所有测试
        for (auto& test : tests_) {
            total++;
            std::cout << "Running " << test.name << "... ";
            
            try {
                test.func();
                std::cout << "PASSED\n";
                passed++;
            } catch (const std::exception& e) {
                std::cout << "FAILED: " << e.what() << "\n";
            }
        }
        
        std::cout << "\nResults: " << passed << "/" << total << " tests passed\n";
        return (passed == total) ? 0 : 1;
    }
    
    static void addTest(const std::string& name, std::function<void()> func) {
        tests_.push_back({name, func});
    }
    
private:
    struct Test {
        std::string name;
        std::function<void()> func;
    };
    
    static std::vector<Test> tests_;
};

std::vector<TestFramework::Test> TestFramework::tests_;

#define TEST(name) \
    void test_##name(); \
    static int test_##name##_registered = TestFramework::addTest(#name, test_##name); \
    void test_##name()

#define ASSERT_TRUE(condition) \
    if (!(condition)) { \
        throw std::runtime_error("Assertion failed: " #condition); \
    }

#define ASSERT_FALSE(condition) \
    if (condition) { \
        throw std::runtime_error("Assertion failed: " #condition); \
    }

#define ASSERT_EQ(expected, actual) \
    if ((expected) != (actual)) { \
        throw std::runtime_error("Assertion failed: expected " + std::to_string(expected) + ", got " + std::to_string(actual)); \
    }

#define ASSERT_NE(notExpected, actual) \
    if ((notExpected) == (actual)) { \
        throw std::runtime_error("Assertion failed: values should not be equal"); \
    }

#define ASSERT_STREQ(expected, actual) \
    if (std::string(expected) != std::string(actual)) { \
        throw std::runtime_error("Assertion failed: expected \"" + std::string(expected) + "\", got \"" + std::string(actual) + "\""); \
    }

// 声明测试函数
void testLexer();
void testParser();
void testGenerator();
void testScanner();

int main() {
    return TestFramework::runTests();
}