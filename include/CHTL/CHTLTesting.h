#pragma once

#include "CHTLNode.h"
#include "CHTLContext.h"
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <functional>
#include <chrono>

namespace CHTL {

/**
 * @brief 测试结果
 */
class TestResult {
public:
    TestResult(const std::string& testName, bool passed, const std::string& message = "");
    ~TestResult() = default;

    std::string getTestName() const { return m_testName; }
    bool isPassed() const { return m_passed; }
    std::string getMessage() const { return m_message; }
    std::chrono::milliseconds getDuration() const { return m_duration; }
    
    void setMessage(const std::string& message) { m_message = message; }
    void setDuration(std::chrono::milliseconds duration) { m_duration = duration; }

private:
    std::string m_testName;
    bool m_passed;
    std::string m_message;
    std::chrono::milliseconds m_duration;
};

/**
 * @brief 测试套件
 */
class TestSuite {
public:
    TestSuite(const std::string& name);
    ~TestSuite() = default;

    std::string getName() const { return m_name; }
    void addTest(const std::string& testName, std::function<TestResult()> testFunction);
    std::vector<TestResult> runAllTests();
    std::vector<TestResult> runTest(const std::string& testName);
    
    // 测试统计
    size_t getTotalTests() const { return m_tests.size(); }
    size_t getPassedTests() const;
    size_t getFailedTests() const;
    double getPassRate() const;

private:
    std::string m_name;
    std::unordered_map<std::string, std::function<TestResult()>> m_tests;
    std::vector<TestResult> m_results;
};

/**
 * @brief 单元测试
 */
class UnitTest {
public:
    explicit UnitTest(std::shared_ptr<CHTLContext> context);
    ~UnitTest() = default;

    // 词法分析器测试
    TestResult testLexerBasic();
    TestResult testLexerComments();
    TestResult testLexerStrings();
    TestResult testLexerKeywords();
    
    // 语法分析器测试
    TestResult testParserBasic();
    TestResult testParserElements();
    TestResult testParserAttributes();
    TestResult testParserTemplates();
    
    // 代码生成器测试
    TestResult testGeneratorBasic();
    TestResult testGeneratorHTML();
    TestResult testGeneratorCSS();
    TestResult testGeneratorJavaScript();
    
    // 模板系统测试
    TestResult testTemplateSystem();
    TestResult testTemplateInheritance();
    TestResult testTemplateVariables();
    
    // 自定义系统测试
    TestResult testCustomSystem();
    TestResult testCustomSpecialization();
    TestResult testCustomOperations();
    
    // 原始嵌入测试
    TestResult testOriginEmbed();
    TestResult testOriginHTML();
    TestResult testOriginCSS();
    TestResult testOriginJavaScript();
    
    // 导入系统测试
    TestResult testImportSystem();
    TestResult testNamespaceManagement();
    TestResult testPathResolution();
    
    // 约束系统测试
    TestResult testConstraintSystem();
    TestResult testExactConstraints();
    TestResult testTypeConstraints();
    TestResult testGlobalConstraints();
    
    // 配置组测试
    TestResult testConfigSystem();
    TestResult testConfigValidation();
    TestResult testConfigApplication();
    
    // CHTL JS 测试
    TestResult testCHTLJSBasic();
    TestResult testCHTLJSFileLoader();
    TestResult testCHTLJSSelectors();
    TestResult testCHTLJSEventListeners();
    TestResult testCHTLJSAnimations();
    TestResult testCHTLJSVirtualObjects();
    TestResult testCHTLJSRouter();
    
    // 模块系统测试
    TestResult testModuleSystem();
    TestResult testCMODModule();
    TestResult testCJMODModule();
    TestResult testHybridModule();
    
    // 扫描器测试
    TestResult testScannerBasic();
    TestResult testScannerPlaceholders();
    TestResult testScannerBoundaries();
    
    // CJMOD API 测试
    TestResult testCJMODAPIBasic();
    TestResult testSyntaxAnalyzer();
    TestResult testParameterBinder();
    TestResult testCJMODScanner();
    TestResult testCJMODGenerator();
    
    // 命令行工具测试
    TestResult testCLIBasic();
    TestResult testCLIParsing();
    TestResult testCLICommands();

private:
    std::shared_ptr<CHTLContext> m_context;
    
    // 辅助方法
    bool compareStrings(const std::string& expected, const std::string& actual) const;
    bool compareFiles(const std::string& expectedFile, const std::string& actualFile) const;
    std::string readTestFile(const std::string& filename) const;
    void writeTestFile(const std::string& filename, const std::string& content) const;
};

/**
 * @brief 集成测试
 */
class IntegrationTest {
public:
    explicit IntegrationTest(std::shared_ptr<CHTLContext> context);
    ~IntegrationTest() = default;

    // 完整编译流程测试
    TestResult testFullCompilation();
    TestResult testCompilationWithTemplates();
    TestResult testCompilationWithCustoms();
    TestResult testCompilationWithImports();
    TestResult testCompilationWithConstraints();
    TestResult testCompilationWithConfig();
    
    // 模块集成测试
    TestResult testModuleIntegration();
    TestResult testCMODIntegration();
    TestResult testCJMODIntegration();
    TestResult testHybridIntegration();
    
    // 系统集成测试
    TestResult testSystemIntegration();
    TestResult testCLIIntegration();
    TestResult testAPIIntegration();

private:
    std::shared_ptr<CHTLContext> m_context;
    
    // 辅助方法
    std::string createTestProject() const;
    void cleanupTestProject() const;
    bool validateCompilationOutput(const std::string& output) const;
};

/**
 * @brief 功能测试
 */
class FunctionalTest {
public:
    explicit FunctionalTest(std::shared_ptr<CHTLContext> context);
    ~FunctionalTest() = default;

    // 基础功能测试
    TestResult testBasicHTMLGeneration();
    TestResult testBasicCSSGeneration();
    TestResult testBasicJavaScriptGeneration();
    
    // 高级功能测试
    TestResult testTemplateFunctionality();
    TestResult testCustomFunctionality();
    TestResult testImportFunctionality();
    TestResult testConstraintFunctionality();
    TestResult testConfigFunctionality();
    
    // CHTL JS 功能测试
    TestResult testCHTLJSFunctionality();
    TestResult testFileLoaderFunctionality();
    TestResult testSelectorFunctionality();
    TestResult testEventListenerFunctionality();
    TestResult testAnimationFunctionality();
    TestResult testVirtualObjectFunctionality();
    TestResult testRouterFunctionality();
    
    // 模块功能测试
    TestResult testModuleFunctionality();
    TestResult testCMODFunctionality();
    TestResult testCJMODFunctionality();
    TestResult testHybridFunctionality();
    
    // API 功能测试
    TestResult testAPIFunctionality();
    TestResult testSyntaxAnalysisFunctionality();
    TestResult testParameterBindingFunctionality();
    TestResult testCodeGenerationFunctionality();
    TestResult testScanningFunctionality();

private:
    std::shared_ptr<CHTLContext> m_context;
    
    // 辅助方法
    bool testFeature(const std::string& featureName, std::function<bool()> testFunction) const;
    std::string generateTestInput(const std::string& feature) const;
    bool validateTestOutput(const std::string& feature, const std::string& output) const;
};

/**
 * @brief 测试运行器
 */
class TestRunner {
public:
    explicit TestRunner(std::shared_ptr<CHTLContext> context);
    ~TestRunner() = default;

    // 测试运行
    void runAllTests();
    void runUnitTests();
    void runIntegrationTests();
    void runFunctionalTests();
    void runTestSuite(const std::string& suiteName);
    void runTest(const std::string& suiteName, const std::string& testName);
    
    // 测试报告
    void generateReport();
    void generateHTMLReport();
    void generateJSONReport();
    void generateXMLReport();
    
    // 测试配置
    void setVerbose(bool verbose) { m_verbose = verbose; }
    void setStopOnFailure(bool stop) { m_stopOnFailure = stop; }
    void setOutputDirectory(const std::string& dir) { m_outputDirectory = dir; }

private:
    std::shared_ptr<CHTLContext> m_context;
    std::unique_ptr<UnitTest> m_unitTest;
    std::unique_ptr<IntegrationTest> m_integrationTest;
    std::unique_ptr<FunctionalTest> m_functionalTest;
    std::unordered_map<std::string, std::unique_ptr<TestSuite>> m_testSuites;
    
    bool m_verbose;
    bool m_stopOnFailure;
    std::string m_outputDirectory;
    
    // 辅助方法
    void initializeTestSuites();
    void printTestResult(const TestResult& result) const;
    void printTestSummary() const;
    std::string formatDuration(std::chrono::milliseconds duration) const;
};

/**
 * @brief 性能测试
 */
class PerformanceTest {
public:
    explicit PerformanceTest(std::shared_ptr<CHTLContext> context);
    ~PerformanceTest() = default;

    // 性能测试
    TestResult testLexerPerformance();
    TestResult testParserPerformance();
    TestResult testGeneratorPerformance();
    TestResult testTemplatePerformance();
    TestResult testImportPerformance();
    TestResult testModulePerformance();
    
    // 内存使用测试
    TestResult testMemoryUsage();
    TestResult testMemoryLeaks();
    
    // 并发测试
    TestResult testConcurrency();
    TestResult testThreadSafety();

private:
    std::shared_ptr<CHTLContext> m_context;
    
    // 辅助方法
    std::chrono::milliseconds measureExecutionTime(std::function<void()> function) const;
    size_t measureMemoryUsage() const;
    bool checkForMemoryLeaks() const;
};

/**
 * @brief 测试工具
 */
class TestUtils {
public:
    static std::string generateRandomString(size_t length);
    static std::string generateRandomCHTLCode(size_t complexity);
    static std::string createTempFile(const std::string& content);
    static void deleteTempFile(const std::string& filename);
    static bool compareFiles(const std::string& file1, const std::string& file2);
    static std::string readFile(const std::string& filename);
    static void writeFile(const std::string& filename, const std::string& content);
    static std::vector<std::string> listFiles(const std::string& directory);
    static void createDirectory(const std::string& directory);
    static void deleteDirectory(const std::string& directory);
};

} // namespace CHTL