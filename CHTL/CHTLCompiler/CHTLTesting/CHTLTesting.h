#ifndef CHTL_TESTING_H
#define CHTL_TESTING_H

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <chrono>
#include <memory>

namespace CHTL {

// Test result structure
struct TestResult {
    std::string test_name;
    bool passed;
    std::string error_message;
    std::chrono::milliseconds duration;
    
    TestResult(const std::string& name, bool pass, const std::string& error = "", 
               std::chrono::milliseconds dur = std::chrono::milliseconds(0))
        : test_name(name), passed(pass), error_message(error), duration(dur) {}
};

// Test suite base class
class TestSuite {
public:
    virtual ~TestSuite() = default;
    virtual void runTests() = 0;
    virtual std::string getSuiteName() const = 0;
    
    void addTest(const std::string& name, std::function<void()> test_func);
    void runTest(const std::string& name, std::function<void()> test_func);
    void assertTrue(bool condition, const std::string& message = "");
    void assertFalse(bool condition, const std::string& message = "");
    void assertEqual(const std::string& expected, const std::string& actual, const std::string& message = "");
    void assertNotEqual(const std::string& expected, const std::string& actual, const std::string& message = "");
    void assertThrows(std::function<void()> func, const std::string& message = "");
    
    std::vector<TestResult> getResults() const { return results; }
    int getPassedCount() const;
    int getFailedCount() const;
    int getTotalCount() const;
    
protected:
    std::vector<TestResult> results;
    std::string current_test_name;
};

// Utility test suite
class UtilTest : public TestSuite {
public:
    void runTests() override;
    std::string getSuiteName() const override { return "UtilTest"; }
    
private:
    void testStringUtil();
    void testFileSystem();
    void testTimeUtil();
    void testLogUtil();
    void testValidationUtil();
    void testZipUtil();
};

// Token test suite
class TokenTest : public TestSuite {
public:
    void runTests() override;
    std::string getSuiteName() const override { return "TokenTest"; }
    
private:
    void testTokenCreation();
    void testTokenComparison();
    void testTokenToString();
    void testTokenTypes();
};

// AST test suite
class AstTest : public TestSuite {
public:
    void runTests() override;
    std::string getSuiteName() const override { return "AstTest"; }
    
private:
    void testElementNode();
    void testTextNode();
    void testAttributeNode();
    void testStyleNode();
    void testTemplateNode();
    void testCustomNode();
    void testOriginNode();
    void testImportNode();
    void testNamespaceNode();
    void testConstraintNode();
    void testConfigurationNode();
};

// Lexer test suite
class LexerTest : public TestSuite {
public:
    void runTests() override;
    std::string getSuiteName() const override { return "LexerTest"; }
    
private:
    void testBasicTokens();
    void testCommentTokens();
    void testStringTokens();
    void testNumberTokens();
    void testIdentifierTokens();
    void testOperatorTokens();
    void testComplexInput();
};

// Parser test suite
class ParserTest : public TestSuite {
public:
    void runTests() override;
    std::string getSuiteName() const override { return "ParserTest"; }
    
private:
    void testBasicParsing();
    void testElementParsing();
    void testAttributeParsing();
    void testStyleParsing();
    void testTemplateParsing();
    void testCustomParsing();
    void testComplexParsing();
};

// Generator test suite
class GeneratorTest : public TestSuite {
public:
    void runTests() override;
    std::string getSuiteName() const override { return "GeneratorTest"; }
    
private:
    void testBasicGeneration();
    void testElementGeneration();
    void testAttributeGeneration();
    void testStyleGeneration();
    void testTemplateGeneration();
    void testCustomGeneration();
    void testComplexGeneration();
};

// Style system test suite
class StyleTest : public TestSuite {
public:
    void runTests() override;
    std::string getSuiteName() const override { return "StyleTest"; }
    
private:
    void testInlineStyles();
    void testAutoClassId();
    void testContextInference();
    void testAttributeConditionals();
    void testChainedOperators();
    void testPointingAttributes();
};

// Template system test suite
class TemplateTest : public TestSuite {
public:
    void runTests() override;
    std::string getSuiteName() const override { return "TemplateTest"; }
    
private:
    void testStyleGroupTemplates();
    void testElementTemplates();
    void testVarGroupTemplates();
    void testTemplateInheritance();
    void testTemplateCombination();
};

// Custom system test suite
class CustomTest : public TestSuite {
public:
    void runTests() override;
    std::string getSuiteName() const override { return "CustomTest"; }
    
private:
    void testStyleGroupSpecialization();
    void testElementSpecialization();
    void testVarGroupSpecialization();
    void testAttributeDeletion();
    void testInheritanceDeletion();
    void testIndexAccess();
    void testElementInsertion();
    void testElementDeletion();
};

// Origin embedding test suite
class OriginTest : public TestSuite {
public:
    void runTests() override;
    std::string getSuiteName() const override { return "OriginTest"; }
    
private:
    void testHtmlEmbedding();
    void testCssEmbedding();
    void testJavaScriptEmbedding();
    void testNamedOriginEmbedding();
    void testOriginProcessing();
};

// Import system test suite
class ImportTest : public TestSuite {
public:
    void runTests() override;
    std::string getSuiteName() const override { return "ImportTest"; }
    
private:
    void testHtmlImport();
    void testCssImport();
    void testJavaScriptImport();
    void testChltlImport();
    void testPreciseImport();
    void testTypeImport();
    void testWildcardImport();
    void testPathEquivalence();
};

// Namespace system test suite
class NamespaceTest : public TestSuite {
public:
    void runTests() override;
    std::string getSuiteName() const override { return "NamespaceTest"; }
    
private:
    void testNamespaceCreation();
    void testNamespaceNesting();
    void testNamespaceMerging();
    void testDefaultNamespace();
    void testNamespaceConflictDetection();
    void testModulePollutionProtection();
};

// Constraint system test suite
class ConstraintTest : public TestSuite {
public:
    void runTests() override;
    std::string getSuiteName() const override { return "ConstraintTest"; }
    
private:
    void testElementConstraints();
    void testTypeConstraints();
    void testGlobalConstraints();
    void testExceptKeyword();
    void testConstraintValidation();
};

// Configuration system test suite
class ConfigurationTest : public TestSuite {
public:
    void runTests() override;
    std::string getSuiteName() const override { return "ConfigurationTest"; }
    
private:
    void testConfigurationGroups();
    void testNamedConfigurationGroups();
    void testKeywordCustomization();
    void testAutoClassIdDisable();
    void testDefaultNamespaceDisable();
    void testCustomOriginTypes();
    void testUseKeyword();
};

// CHTL JS test suite
class CHTLJSTest : public TestSuite {
public:
    void runTests() override;
    std::string getSuiteName() const override { return "CHTLJSTest"; }
    
private:
    void testFileLoader();
    void testLocalScriptBlocks();
    void testEnhancedSelectors();
    void testEnhancedListeners();
    void testEventDelegation();
    void testAnimations();
    void testVirtualObjects();
    void testRouting();
    void testDynamicAttributeConditionals();
};

// Module system test suite
class ModuleTest : public TestSuite {
public:
    void runTests() override;
    std::string getSuiteName() const override { return "ModuleTest"; }
    
private:
    void testCMODModules();
    void testCJMODModules();
    void testMixedModules();
    void testWildcardImports();
    void testOfficialModules();
    void testModuleSearchPaths();
    void testModuleDependencies();
};

// CJMOD API test suite
class CJMODAPITest : public TestSuite {
public:
    void runTests() override;
    std::string getSuiteName() const override { return "CJMODAPITest"; }
    
private:
    void testSyntaxAnalysis();
    void testArgBinding();
    void testCJMODScanner();
    void testCJMODGenerator();
    void testAtomArgPlaceholders();
    void testCHTLJSFunction();
    void testVirtualObjectBinding();
};

// Unified scanner test suite
class UnifiedScannerTest : public TestSuite {
public:
    void runTests() override;
    std::string getSuiteName() const override { return "UnifiedScannerTest"; }
    
private:
    void testVariableLengthSlicing();
    void testIntelligentExpansion();
    void testPlaceholderMechanism();
    void testBroadStrictJudgment();
    void testScannerIntegration();
};

// Compiler dispatcher test suite
class CompilerDispatcherTest : public TestSuite {
public:
    void runTests() override;
    std::string getSuiteName() const override { return "CompilerDispatcherTest"; }
    
private:
    void testCompilerSelection();
    void testCHTLCompilation();
    void testCHTLJSCompilation();
    void testCSSCompilation();
    void testJavaScriptCompilation();
    void testMixedCompilation();
    void testCompilerVersion();
    void testSupportedExtensions();
};

// Code merger test suite
class CodeMergerTest : public TestSuite {
public:
    void runTests() override;
    std::string getSuiteName() const override { return "CodeMergerTest"; }
    
private:
    void testFragmentDetection();
    void testHTMLMerging();
    void testCSSMerging();
    void testJavaScriptMerging();
    void testCHTLMerging();
    void testCHTLJSMerging();
    void testMixedMerging();
    void testMergeStrategies();
};

// IO stream test suite
class IOStreamTest : public TestSuite {
public:
    void runTests() override;
    std::string getSuiteName() const override { return "IOStreamTest"; }
    
private:
    void testFileStream();
    void testStringStream();
    void testStreamModes();
    void testStreamStatus();
    void testPathUtilities();
    void testStreamOperations();
};

// Test runner
class TestRunner {
public:
    TestRunner();
    ~TestRunner();
    
    void addTestSuite(std::unique_ptr<TestSuite> suite);
    void runAllTests();
    void runTestSuite(const std::string& suite_name);
    void runTest(const std::string& suite_name, const std::string& test_name);
    
    void setOutputFile(const std::string& file_path);
    void setVerbose(bool verbose);
    void setStopOnFailure(bool stop_on_failure);
    
    std::vector<TestResult> getAllResults() const;
    int getTotalPassed() const;
    int getTotalFailed() const;
    int getTotalTests() const;
    
    void printSummary() const;
    void printDetailedResults() const;
    void exportResults(const std::string& file_path) const;
    
private:
    std::vector<std::unique_ptr<TestSuite>> test_suites;
    std::string output_file;
    bool verbose;
    bool stop_on_failure;
    std::vector<TestResult> all_results;
    
    void printTestResult(const TestResult& result) const;
    void printSuiteResults(const TestSuite& suite) const;
};

} // namespace CHTL

#endif // CHTL_TESTING_H