#include "CHTLTesting.h"
#include "CHTLUtilities/CHTLUtilities.h"
#include "CHTLLexer/CHTLLexer.h"
#include "CHTLParser/CHTLParser.h"
#include "CHTLGenerator/CHTLGenerator.h"
#include "CHTLNode/CHTLNode.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cassert>

namespace CHTL {

// TestSuite implementation
void TestSuite::addTest(const std::string& /* name */, std::function<void()> /* test_func */) {
    // This is a placeholder - actual implementation would store the test
}

void TestSuite::runTest(const std::string& name, std::function<void()> test_func) {
    current_test_name = name;
    auto start_time = std::chrono::high_resolution_clock::now();
    
    try {
        test_func();
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        results.emplace_back(name, true, "", duration);
    } catch (const std::exception& e) {
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        results.emplace_back(name, false, e.what(), duration);
    } catch (...) {
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        results.emplace_back(name, false, "Unknown error", duration);
    }
}

void TestSuite::assertTrue(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error("Assertion failed: " + message);
    }
}

void TestSuite::assertFalse(bool condition, const std::string& message) {
    if (condition) {
        throw std::runtime_error("Assertion failed: " + message);
    }
}

void TestSuite::assertEqual(const std::string& expected, const std::string& actual, const std::string& message) {
    if (expected != actual) {
        throw std::runtime_error("Assertion failed: " + message + " (expected: " + expected + ", actual: " + actual + ")");
    }
}

void TestSuite::assertNotEqual(const std::string& expected, const std::string& actual, const std::string& message) {
    if (expected == actual) {
        throw std::runtime_error("Assertion failed: " + message + " (values should not be equal)");
    }
}

void TestSuite::assertThrows(std::function<void()> func, const std::string& message) {
    bool threw = false;
    try {
        func();
    } catch (...) {
        threw = true;
    }
    if (!threw) {
        throw std::runtime_error("Assertion failed: " + message + " (expected exception was not thrown)");
    }
}

int TestSuite::getPassedCount() const {
    int count = 0;
    for (const auto& result : results) {
        if (result.passed) count++;
    }
    return count;
}

int TestSuite::getFailedCount() const {
    int count = 0;
    for (const auto& result : results) {
        if (!result.passed) count++;
    }
    return count;
}

int TestSuite::getTotalCount() const {
    return results.size();
}

// UtilTest implementation
void UtilTest::runTests() {
    runTest("testStringUtil", [this]() { testStringUtil(); });
    runTest("testFileSystem", [this]() { testFileSystem(); });
    runTest("testTimeUtil", [this]() { testTimeUtil(); });
    runTest("testLogUtil", [this]() { testLogUtil(); });
    runTest("testValidationUtil", [this]() { testValidationUtil(); });
    runTest("testZipUtil", [this]() { testZipUtil(); });
}

void UtilTest::testStringUtil() {
    // Test basic string operations
    assertEqual("hello world", StringUtil::toLower("HELLO WORLD"), "toLower test");
    assertEqual("HELLO WORLD", StringUtil::toUpper("hello world"), "toUpper test");
    assertEqual("Hello World", StringUtil::capitalize("hello world"), "capitalize test");
    
    // Test string manipulation
    assertEqual("hello", StringUtil::trim("  hello  "), "trim test");
    assertEqual("hello", StringUtil::ltrim("  hello"), "ltrim test");
    assertEqual("hello", StringUtil::rtrim("hello  "), "rtrim test");
    
    // Test string splitting
    std::vector<std::string> parts = StringUtil::split("a,b,c", ",");
    assertTrue(parts.size() == 3, "split test size");
    assertEqual("a", parts[0], "split test first element");
    assertEqual("b", parts[1], "split test second element");
    assertEqual("c", parts[2], "split test third element");
    
    // Test string joining
    std::vector<std::string> words = {"hello", "world", "test"};
    assertEqual("hello,world,test", StringUtil::join(words, ","), "join test");
    
    // Test string replacement
    assertEqual("hello world", StringUtil::replaceAll("hello test", "test", "world"), "replaceAll test");
    
    // Test string formatting
    assertEqual("Hello John, you are 25 years old", 
                StringUtil::format("Hello %s, you are %d years old", "John", 25), "format test");
}

void UtilTest::testFileSystem() {
    // Test file existence (assuming test files exist)
    assertTrue(FileSystem::exists("/workspace/CHTL"), "file system exists test");
    assertTrue(FileSystem::isDirectory("/workspace/CHTL"), "isDirectory test");
    assertFalse(FileSystem::isFile("/workspace/CHTL"), "isFile test for directory");
    
    // Test file operations
    std::string test_file = "/tmp/chtl_test.txt";
    std::string test_content = "Hello, CHTL!";
    
    // Write test file
    assertTrue(FileSystem::writeFile(test_file, test_content), "writeFile test");
    assertTrue(FileSystem::exists(test_file), "file exists after write");
    assertTrue(FileSystem::isFile(test_file), "isFile test");
    
    // Read test file
    std::string read_content = FileSystem::readFile(test_file);
    assertEqual(test_content, read_content, "readFile test");
    
    // Clean up
    FileSystem::removeFile(test_file);
}

void UtilTest::testTimeUtil() {
    // Test time utilities
    std::string date = TimeUtil::getCurrentDate();
    assertTrue(date.length() >= 10, "getCurrentDate length test");
    
    std::string time = TimeUtil::getCurrentTime();
    assertTrue(time.length() >= 8, "getCurrentTime length test");
    
    std::string datetime = TimeUtil::getCurrentDateTime();
    assertTrue(datetime.length() >= 19, "getCurrentDateTime length test");
    
    // Test timestamp
    long long timestamp = TimeUtil::getTimestamp();
    assertTrue(timestamp > 0, "getTimestamp test");
    
    // Test sleep
    auto start = std::chrono::high_resolution_clock::now();
    TimeUtil::sleepMs(100); // 100ms
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    assertTrue(duration.count() >= 100, "sleep test");
}

void UtilTest::testLogUtil() {
    // Test log level setting
    LogUtil::setLogLevel(LogUtil::DEBUG);
    assertTrue(LogUtil::isEnabled(LogUtil::DEBUG), "log level DEBUG enabled test");
    assertTrue(LogUtil::isEnabled(LogUtil::INFO), "log level INFO enabled test");
    assertTrue(LogUtil::isEnabled(LogUtil::WARNING), "log level WARNING enabled test");
    assertTrue(LogUtil::isEnabled(LogUtil::ERROR), "log level ERROR enabled test");
    assertTrue(LogUtil::isEnabled(LogUtil::CRITICAL), "log level CRITICAL enabled test");
    
    // Test log level string conversion
    assertEqual("DEBUG", LogUtil::getLogLevelString(LogUtil::DEBUG), "log level string DEBUG test");
    assertEqual("INFO", LogUtil::getLogLevelString(LogUtil::INFO), "log level string INFO test");
    assertEqual("WARNING", LogUtil::getLogLevelString(LogUtil::WARNING), "log level string WARNING test");
    assertEqual("ERROR", LogUtil::getLogLevelString(LogUtil::ERROR), "log level string ERROR test");
    assertEqual("CRITICAL", LogUtil::getLogLevelString(LogUtil::CRITICAL), "log level string CRITICAL test");
    
    // Test log level from string
    assertTrue(LogUtil::getLogLevelFromString("DEBUG") == LogUtil::DEBUG, "log level from string DEBUG test");
    assertTrue(LogUtil::getLogLevelFromString("INFO") == LogUtil::INFO, "log level from string INFO test");
    assertTrue(LogUtil::getLogLevelFromString("WARNING") == LogUtil::WARNING, "log level from string WARNING test");
    assertTrue(LogUtil::getLogLevelFromString("ERROR") == LogUtil::ERROR, "log level from string ERROR test");
    assertTrue(LogUtil::getLogLevelFromString("CRITICAL") == LogUtil::CRITICAL, "log level from string CRITICAL test");
}

void UtilTest::testValidationUtil() {
    // Test email validation
    assertTrue(ValidationUtil::isValidEmail("test@example.com"), "valid email test");
    assertTrue(ValidationUtil::isValidEmail("user.name@domain.co.uk"), "valid email with subdomain test");
    assertFalse(ValidationUtil::isValidEmail("invalid-email"), "invalid email test");
    assertFalse(ValidationUtil::isValidEmail("@example.com"), "invalid email missing local part test");
    
    // Test URL validation
    assertTrue(ValidationUtil::isValidUrl("https://www.example.com"), "valid HTTPS URL test");
    assertTrue(ValidationUtil::isValidUrl("http://example.com/path"), "valid HTTP URL test");
    assertFalse(ValidationUtil::isValidUrl("not-a-url"), "invalid URL test");
    assertFalse(ValidationUtil::isValidUrl("ftp://example.com"), "invalid protocol URL test");
    
    // Test IP address validation
    assertTrue(ValidationUtil::isValidIpAddress("192.168.1.1"), "valid IP address test");
    assertTrue(ValidationUtil::isValidIpAddress("127.0.0.1"), "valid localhost IP test");
    assertFalse(ValidationUtil::isValidIpAddress("256.1.1.1"), "invalid IP address test");
    assertFalse(ValidationUtil::isValidIpAddress("192.168.1"), "incomplete IP address test");
    
    // Test domain validation
    assertTrue(ValidationUtil::isValidDomain("example.com"), "valid domain test");
    assertTrue(ValidationUtil::isValidDomain("sub.example.com"), "valid subdomain test");
    assertFalse(ValidationUtil::isValidDomain("-invalid.com"), "invalid domain test");
    assertFalse(ValidationUtil::isValidDomain(""), "empty domain test");
}

void UtilTest::testZipUtil() {
    // Test zip file creation and extraction
    std::string test_dir = "/tmp/chtl_zip_test";
    std::string zip_file = "/tmp/chtl_test.zip";
    
    // Create test directory
    FileSystem::createDirectories(test_dir);
    FileSystem::writeFile(test_dir + "/test1.txt", "Hello from test1");
    FileSystem::writeFile(test_dir + "/test2.txt", "Hello from test2");
    
    // Test zip creation
    assertTrue(ZipUtil::create(zip_file, test_dir), "createZip test");
    assertTrue(FileSystem::exists(zip_file), "zip file exists test");
    
    // Test zip extraction
    std::string extract_dir = "/tmp/chtl_zip_extract";
    assertTrue(ZipUtil::extract(zip_file, extract_dir), "extractZip test");
    assertTrue(FileSystem::exists(extract_dir), "extract directory exists test");
    
    // Test zip file listing
    std::vector<std::string> files = ZipUtil::listFiles(zip_file);
    assertTrue(files.size() >= 2, "zip file listing test");
    
    // Clean up
    FileSystem::removeFile(zip_file);
    FileSystem::removeDirectory(test_dir);
    FileSystem::removeDirectory(extract_dir);
}

// TokenTest implementation
void TokenTest::runTests() {
    runTest("testTokenCreation", [this]() { testTokenCreation(); });
    runTest("testTokenComparison", [this]() { testTokenComparison(); });
    runTest("testTokenToString", [this]() { testTokenToString(); });
    runTest("testTokenTypes", [this]() { testTokenTypes(); });
}

void TokenTest::testTokenCreation() {
    Token token(TokenType::Text, "test", 1, 1, 0);
    assertEqual("test", token.value, "token value test");
    assertTrue(token.type == TokenType::Text, "token type test");
    assertTrue(token.line == 1, "token line test");
    assertTrue(token.column == 1, "token column test");
}

void TokenTest::testTokenComparison() {
    Token token1(TokenType::Text, "test", 1, 1, 0);
    Token token2(TokenType::Text, "test", 1, 1, 0);
    Token token3(TokenType::Style, "test", 1, 1, 0);
    
    assertTrue(token1 == token2, "token equality test");
    assertFalse(token1 == token3, "token inequality test");
}

void TokenTest::testTokenToString() {
    Token token(TokenType::Text, "test", 1, 1, 0);
    std::string str = token.toString();
    assertTrue(str.find("Text") != std::string::npos, "token toString test");
    assertTrue(str.find("test") != std::string::npos, "token toString value test");
}

void TokenTest::testTokenTypes() {
    // Test various token types
    Token text_token(TokenType::Text, "test", 1, 1, 0);
    Token style_token(TokenType::Style, "hello", 1, 1, 0);
    Token script_token(TokenType::Script, "123", 1, 1, 0);
    Token template_token(TokenType::Template, "+", 1, 1, 0);
    
    assertTrue(text_token.type == TokenType::Text, "text token type test");
    assertTrue(style_token.type == TokenType::Style, "style token type test");
    assertTrue(script_token.type == TokenType::Script, "script token type test");
    assertTrue(template_token.type == TokenType::Template, "template token type test");
}

// ASTTest implementation
void AstTest::runTests() {
    runTest("testElementNode", [this]() { testElementNode(); });
    runTest("testTextNode", [this]() { testTextNode(); });
    runTest("testAttributeNode", [this]() { testAttributeNode(); });
    runTest("testStyleNode", [this]() { testStyleNode(); });
    runTest("testTemplateNode", [this]() { testTemplateNode(); });
    runTest("testCustomNode", [this]() { testCustomNode(); });
    runTest("testOriginNode", [this]() { testOriginNode(); });
    runTest("testImportNode", [this]() { testImportNode(); });
    runTest("testNamespaceNode", [this]() { testNamespaceNode(); });
    runTest("testConstraintNode", [this]() { testConstraintNode(); });
    runTest("testConfigurationNode", [this]() { testConfigurationNode(); });
}

void AstTest::testElementNode() {
    ElementNode element("div");
    assertEqual("div", element.tagName, "element tag name test");
    assertTrue(element.attributes.empty(), "element attributes empty test");
    assertTrue(element.children.empty(), "element children empty test");
    
    // Test adding attributes
    element.attributes["class"] = "test-class";
    assertEqual("test-class", element.attributes["class"], "element attribute test");
    
    // Test adding children
    auto textNode = std::make_unique<TextNode>("Hello");
    element.children.push_back(std::move(textNode));
    assertTrue(element.children.size() == 1, "element children size test");
}

void AstTest::testTextNode() {
    TextNode text("Hello, World!");
    assertEqual("Hello, World!", text.content, "text node content test");
    assertTrue(text.toHTML() == "Hello, World!", "text node toHTML test");
}

void AstTest::testAttributeNode() {
    AttributeNode attr("class", "test-class");
    assertEqual("class", attr.name, "attribute node name test");
    assertEqual("test-class", attr.value, "attribute node value test");
}

void AstTest::testStyleNode() {
    StyleNode style;
    style.rules["color"] = "red";
    style.rules["font-size"] = "16px";
    assertEqual("red", style.rules["color"], "style node color rule test");
    assertEqual("16px", style.rules["font-size"], "style node font-size rule test");
}

void AstTest::testTemplateNode() {
    TemplateNode template_node("test-template");
    assertEqual("test-template", template_node.name, "template node name test");
    assertTrue(template_node.content.empty(), "template node content empty test");
}

void AstTest::testCustomNode() {
    CustomNode custom("custom-element");
    assertEqual("custom-element", custom.name, "custom node name test");
    assertTrue(custom.attributes.empty(), "custom node attributes empty test");
}

void AstTest::testOriginNode() {
    OriginNode origin("html", "<div>Hello</div>");
    assertEqual("html", origin.type, "origin node type test");
    assertEqual("<div>Hello</div>", origin.content, "origin node content test");
}

void AstTest::testImportNode() {
    ImportNode import("test.chtl", ImportType::PRECISE);
    assertEqual("test.chtl", import.filePath, "import node file path test");
    assertTrue(import.type == ImportType::PRECISE, "import node type test");
}

void AstTest::testNamespaceNode() {
    NamespaceNode namespace_node("test-namespace");
    assertEqual("test-namespace", namespace_node.name, "namespace node name test");
    assertTrue(namespace_node.elements.empty(), "namespace node elements empty test");
}

void AstTest::testConstraintNode() {
    ConstraintNode constraint("div", ConstraintType::ELEMENT);
    assertEqual("div", constraint.target, "constraint node target test");
    assertTrue(constraint.type == ConstraintType::ELEMENT, "constraint node type test");
}

void AstTest::testConfigurationNode() {
    ConfigurationNode config("test-config");
    assertEqual("test-config", config.name, "configuration node name test");
    assertTrue(config.settings.empty(), "configuration node settings empty test");
}

// LexerTest implementation
void LexerTest::runTests() {
    runTest("testBasicTokens", [this]() { testBasicTokens(); });
    runTest("testCommentTokens", [this]() { testCommentTokens(); });
    runTest("testStringTokens", [this]() { testStringTokens(); });
    runTest("testNumberTokens", [this]() { testNumberTokens(); });
    runTest("testIdentifierTokens", [this]() { testIdentifierTokens(); });
    runTest("testOperatorTokens", [this]() { testOperatorTokens(); });
    runTest("testComplexInput", [this]() { testComplexInput(); });
}

void LexerTest::testBasicTokens() {
    CHTLLexer lexer("div class=\"test\"");
    auto tokens = lexer.tokenize();
    
    assertTrue(tokens.size() >= 3, "basic tokens count test");
    assertTrue(tokens[0].type == TokenType::Text, "first token type test");
    assertEqual("div", tokens[0].value, "first token value test");
}

void LexerTest::testCommentTokens() {
    CHTLLexer lexer("// This is a comment\ndiv");
    auto tokens = lexer.tokenize();
    
    // Comments should be skipped
    assertTrue(tokens.size() >= 1, "comment tokens count test");
    assertTrue(tokens[0].type == TokenType::Text, "token after comment test");
    assertEqual("div", tokens[0].value, "token after comment value test");
}

void LexerTest::testStringTokens() {
    CHTLLexer lexer("\"hello world\" 'single quotes'");
    auto tokens = lexer.tokenize();
    
    assertTrue(tokens.size() >= 2, "string tokens count test");
    assertTrue(tokens[0].getType() == TokenType::Text, "first string token type test");
    assertEqual("hello world", tokens[0].value, "first string token value test");
    assertTrue(tokens[1].type == TokenType::Text, "second string token type test");
    assertEqual("single quotes", tokens[1].value, "second string token value test");
}

void LexerTest::testNumberTokens() {
    CHTLLexer lexer("123 45.67");
    auto tokens = lexer.tokenize();
    
    assertTrue(tokens.size() >= 2, "number tokens count test");
    assertTrue(tokens[0].type == TokenType::Text, "first number token type test");
    assertEqual("123", tokens[0].value, "first number token value test");
    assertTrue(tokens[1].type == TokenType::Text, "second number token type test");
    assertEqual("45.67", tokens[1].value, "second number token value test");
}

void LexerTest::testIdentifierTokens() {
    CHTLLexer lexer("div class id");
    auto tokens = lexer.tokenize();
    
    assertTrue(tokens.size() >= 3, "identifier tokens count test");
    assertTrue(tokens[0].type == TokenType::Text, "first identifier token type test");
    assertEqual("div", tokens[0].value, "first identifier token value test");
    assertTrue(tokens[1].type == TokenType::Text, "second identifier token type test");
    assertEqual("class", tokens[1].value, "second identifier token value test");
    assertTrue(tokens[2].type == TokenType::Text, "third identifier token type test");
    assertEqual("id", tokens[2].value, "third identifier token value test");
}

void LexerTest::testOperatorTokens() {
    CHTLLexer lexer("+ - * / = == != < >");
    auto tokens = lexer.tokenize();
    
    assertTrue(tokens.size() >= 9, "operator tokens count test");
    assertTrue(tokens[0].type == TokenType::Text, "plus token type test");
    assertTrue(tokens[1].type == TokenType::Text, "minus token type test");
    assertTrue(tokens[2].type == TokenType::Text, "multiply token type test");
    assertTrue(tokens[3].type == TokenType::Text, "divide token type test");
    assertTrue(tokens[4].type == TokenType::Text, "assign token type test");
    assertTrue(tokens[5].type == TokenType::Text, "equal token type test");
    assertTrue(tokens[6].type == TokenType::Text, "not equal token type test");
    assertTrue(tokens[7].type == TokenType::Text, "less than token type test");
    assertTrue(tokens[8].type == TokenType::Text, "greater than token type test");
}

void LexerTest::testComplexInput() {
    CHTLLexer lexer("div class=\"test\" id='main' // comment\n  style { color: red; }");
    auto tokens = lexer.tokenize();
    
    assertTrue(tokens.size() >= 5, "complex input tokens count test");
    assertTrue(tokens[0].type == TokenType::Text, "complex input first token type test");
    assertEqual("div", tokens[0].value, "complex input first token value test");
}

// ParserTest implementation
void ParserTest::runTests() {
    runTest("testBasicParsing", [this]() { testBasicParsing(); });
    runTest("testElementParsing", [this]() { testElementParsing(); });
    runTest("testAttributeParsing", [this]() { testAttributeParsing(); });
    runTest("testStyleParsing", [this]() { testStyleParsing(); });
    runTest("testTemplateParsing", [this]() { testTemplateParsing(); });
    runTest("testCustomParsing", [this]() { testCustomParsing(); });
    runTest("testComplexParsing", [this]() { testComplexParsing(); });
}

void ParserTest::testBasicParsing() {
    CHTLLexer lexer("div");
    auto tokens = lexer.tokenize();
    CHTLParser parser(tokens);
    
    auto ast = parser.parse();
    assertTrue(ast != nullptr, "basic parsing AST creation test");
    assertTrue(ast->children.size() >= 1, "basic parsing children count test");
}

void ParserTest::testElementParsing() {
    CHTLLexer lexer("div class=\"test\"");
    auto tokens = lexer.tokenize();
    CHTLParser parser(tokens);
    
    auto ast = parser.parse();
    assertTrue(ast != nullptr, "element parsing AST creation test");
    assertTrue(ast->children.size() >= 1, "element parsing children count test");
}

void ParserTest::testAttributeParsing() {
    CHTLLexer lexer("div class=\"test\" id='main'");
    auto tokens = lexer.tokenize();
    CHTLParser parser(tokens);
    
    auto ast = parser.parse();
    assertTrue(ast != nullptr, "attribute parsing AST creation test");
    assertTrue(ast->children.size() >= 1, "attribute parsing children count test");
}

void ParserTest::testStyleParsing() {
    CHTLLexer lexer("style { color: red; font-size: 16px; }");
    auto tokens = lexer.tokenize();
    CHTLParser parser(tokens);
    
    auto ast = parser.parse();
    assertTrue(ast != nullptr, "style parsing AST creation test");
    assertTrue(ast->children.size() >= 1, "style parsing children count test");
}

void ParserTest::testTemplateParsing() {
    CHTLLexer lexer("[Template] test { color: red; }");
    auto tokens = lexer.tokenize();
    CHTLParser parser(tokens);
    
    auto ast = parser.parse();
    assertTrue(ast != nullptr, "template parsing AST creation test");
    assertTrue(ast->children.size() >= 1, "template parsing children count test");
}

void ParserTest::testCustomParsing() {
    CHTLLexer lexer("[Custom] test-element { color: red; }");
    auto tokens = lexer.tokenize();
    CHTLParser parser(tokens);
    
    auto ast = parser.parse();
    assertTrue(ast != nullptr, "custom parsing AST creation test");
    assertTrue(ast->children.size() >= 1, "custom parsing children count test");
}

void ParserTest::testComplexParsing() {
    CHTLLexer lexer("div class=\"container\" {\n  h1 { color: red; }\n  p { color: blue; }\n}");
    auto tokens = lexer.tokenize();
    CHTLParser parser(tokens);
    
    auto ast = parser.parse();
    assertTrue(ast != nullptr, "complex parsing AST creation test");
    assertTrue(ast->children.size() >= 1, "complex parsing children count test");
}

// GeneratorTest implementation
void GeneratorTest::runTests() {
    runTest("testBasicGeneration", [this]() { testBasicGeneration(); });
    runTest("testElementGeneration", [this]() { testElementGeneration(); });
    runTest("testAttributeGeneration", [this]() { testAttributeGeneration(); });
    runTest("testStyleGeneration", [this]() { testStyleGeneration(); });
    runTest("testTemplateGeneration", [this]() { testTemplateGeneration(); });
    runTest("testCustomGeneration", [this]() { testCustomGeneration(); });
    runTest("testComplexGeneration", [this]() { testComplexGeneration(); });
}

void GeneratorTest::testBasicGeneration() {
    auto ast = std::make_unique<ASTNode>();
    auto element = std::make_unique<ElementNode>("div");
    element->children.push_back(std::make_unique<TextNode>("Hello, World!"));
    ast->children.push_back(std::move(element));
    
    CHTLGenerator generator;
    std::string html = generator.generateHTML(ast.get());
    
    assertTrue(html.find("<div>") != std::string::npos, "basic generation div tag test");
    assertTrue(html.find("Hello, World!") != std::string::npos, "basic generation text content test");
    assertTrue(html.find("</div>") != std::string::npos, "basic generation closing div tag test");
}

void GeneratorTest::testElementGeneration() {
    auto ast = std::make_unique<ASTNode>();
    auto element = std::make_unique<ElementNode>("div");
    element->attributes["class"] = "test-class";
    element->attributes["id"] = "test-id";
    ast->children.push_back(std::move(element));
    
    CHTLGenerator generator;
    std::string html = generator.generateHTML(ast.get());
    
    assertTrue(html.find("class=\"test-class\"") != std::string::npos, "element generation class attribute test");
    assertTrue(html.find("id=\"test-id\"") != std::string::npos, "element generation id attribute test");
}

void GeneratorTest::testAttributeGeneration() {
    auto ast = std::make_unique<ASTNode>();
    auto element = std::make_unique<ElementNode>("input");
    element->attributes["type"] = "text";
    element->attributes["placeholder"] = "Enter text";
    element->attributes["required"] = "true";
    ast->children.push_back(std::move(element));
    
    CHTLGenerator generator;
    std::string html = generator.generateHTML(ast.get());
    
    assertTrue(html.find("type=\"text\"") != std::string::npos, "attribute generation type test");
    assertTrue(html.find("placeholder=\"Enter text\"") != std::string::npos, "attribute generation placeholder test");
    assertTrue(html.find("required=\"true\"") != std::string::npos, "attribute generation required test");
}

void GeneratorTest::testStyleGeneration() {
    auto ast = std::make_unique<ASTNode>();
    auto style = std::make_unique<StyleNode>();
    style->rules["color"] = "red";
    style->rules["font-size"] = "16px";
    ast->children.push_back(std::move(style));
    
    CHTLGenerator generator;
    std::string html = generator.generateHTML(ast.get());
    
    assertTrue(html.find("color: red") != std::string::npos, "style generation color rule test");
    assertTrue(html.find("font-size: 16px") != std::string::npos, "style generation font-size rule test");
}

void GeneratorTest::testTemplateGeneration() {
    auto ast = std::make_unique<ASTNode>();
    auto template_node = std::make_unique<TemplateNode>("test-template");
    template_node->content = "color: red; font-size: 16px;";
    ast->children.push_back(std::move(template_node));
    
    CHTLGenerator generator;
    std::string html = generator.generateHTML(ast.get());
    
    assertTrue(html.find("test-template") != std::string::npos, "template generation name test");
    assertTrue(html.find("color: red") != std::string::npos, "template generation content test");
}

void GeneratorTest::testCustomGeneration() {
    auto ast = std::make_unique<ASTNode>();
    auto custom = std::make_unique<CustomNode>("custom-element");
    custom->attributes["data-test"] = "value";
    ast->children.push_back(std::move(custom));
    
    CHTLGenerator generator;
    std::string html = generator.generateHTML(ast.get());
    
    assertTrue(html.find("custom-element") != std::string::npos, "custom generation tag name test");
    assertTrue(html.find("data-test=\"value\"") != std::string::npos, "custom generation attribute test");
}

void GeneratorTest::testComplexGeneration() {
    auto ast = std::make_unique<ASTNode>();
    
    // Create a complex structure
    auto div = std::make_unique<ElementNode>("div");
    div->attributes["class"] = "container";
    
    auto h1 = std::make_unique<ElementNode>("h1");
    h1->children.push_back(std::make_unique<TextNode>("Title"));
    div->children.push_back(std::move(h1));
    
    auto p = std::make_unique<ElementNode>("p");
    p->children.push_back(std::make_unique<TextNode>("Content"));
    div->children.push_back(std::move(p));
    
    ast->children.push_back(std::move(div));
    
    CHTLGenerator generator;
    std::string html = generator.generateHTML(ast.get());
    
    assertTrue(html.find("<div class=\"container\">") != std::string::npos, "complex generation container test");
    assertTrue(html.find("<h1>Title</h1>") != std::string::npos, "complex generation h1 test");
    assertTrue(html.find("<p>Content</p>") != std::string::npos, "complex generation p test");
    assertTrue(html.find("</div>") != std::string::npos, "complex generation closing div test");
}

// TestRunner implementation
TestRunner::TestRunner() : verbose(false), stop_on_failure(false) {}

TestRunner::~TestRunner() = default;

void TestRunner::addTestSuite(std::unique_ptr<TestSuite> suite) {
    test_suites.push_back(std::move(suite));
}

void TestRunner::runAllTests() {
    all_results.clear();
    
    for (auto& suite : test_suites) {
        std::cout << "Running " << suite->getSuiteName() << "..." << std::endl;
        suite->runTests();
        
        auto suite_results = suite->getResults();
        all_results.insert(all_results.end(), suite_results.begin(), suite_results.end());
        
        printSuiteResults(*suite);
        
        if (stop_on_failure && suite->getFailedCount() > 0) {
            std::cout << "Stopping on first failure." << std::endl;
            break;
        }
    }
    
    printSummary();
}

void TestRunner::runTestSuite(const std::string& suite_name) {
    for (auto& suite : test_suites) {
        if (suite->getSuiteName() == suite_name) {
            std::cout << "Running " << suite_name << "..." << std::endl;
            suite->runTests();
            printSuiteResults(*suite);
            return;
        }
    }
    std::cout << "Test suite '" << suite_name << "' not found." << std::endl;
}

void TestRunner::runTest(const std::string& suite_name, const std::string& test_name) {
    for (auto& suite : test_suites) {
        if (suite->getSuiteName() == suite_name) {
            std::cout << "Running " << suite_name << "::" << test_name << "..." << std::endl;
            // Individual test running would need to be implemented in TestSuite
            return;
        }
    }
    std::cout << "Test suite '" << suite_name << "' not found." << std::endl;
}

void TestRunner::setOutputFile(const std::string& file_path) {
    output_file = file_path;
}

void TestRunner::setVerbose(bool verbose) {
    this->verbose = verbose;
}

void TestRunner::setStopOnFailure(bool stop_on_failure) {
    this->stop_on_failure = stop_on_failure;
}

std::vector<TestResult> TestRunner::getAllResults() const {
    return all_results;
}

int TestRunner::getTotalPassed() const {
    int count = 0;
    for (const auto& result : all_results) {
        if (result.passed) count++;
    }
    return count;
}

int TestRunner::getTotalFailed() const {
    int count = 0;
    for (const auto& result : all_results) {
        if (!result.passed) count++;
    }
    return count;
}

int TestRunner::getTotalTests() const {
    return all_results.size();
}

void TestRunner::printSummary() const {
    std::cout << "\n=== Test Summary ===" << std::endl;
    std::cout << "Total tests: " << getTotalTests() << std::endl;
    std::cout << "Passed: " << getTotalPassed() << std::endl;
    std::cout << "Failed: " << getTotalFailed() << std::endl;
    
    if (getTotalFailed() > 0) {
        std::cout << "\nFailed tests:" << std::endl;
        for (const auto& result : all_results) {
            if (!result.passed) {
                printTestResult(result);
            }
        }
    }
}

void TestRunner::printDetailedResults() const {
    std::cout << "\n=== Detailed Test Results ===" << std::endl;
    for (const auto& result : all_results) {
        printTestResult(result);
    }
}

void TestRunner::exportResults(const std::string& file_path) const {
    std::ofstream file(file_path);
    if (!file.is_open()) {
        std::cout << "Failed to open output file: " << file_path << std::endl;
        return;
    }
    
    file << "Test Results\n";
    file << "============\n\n";
    
    for (const auto& result : all_results) {
        file << "Test: " << result.test_name << "\n";
        file << "Status: " << (result.passed ? "PASSED" : "FAILED") << "\n";
        file << "Duration: " << result.duration.count() << "ms\n";
        if (!result.passed) {
            file << "Error: " << result.error_message << "\n";
        }
        file << "\n";
    }
    
    file.close();
    std::cout << "Results exported to: " << file_path << std::endl;
}

void TestRunner::printTestResult(const TestResult& result) const {
    std::cout << "  " << (result.passed ? "PASS" : "FAIL") << " - " << result.test_name;
    if (!result.passed) {
        std::cout << " (" << result.error_message << ")";
    }
    std::cout << " [" << result.duration.count() << "ms]" << std::endl;
}

void TestRunner::printSuiteResults(const TestSuite& suite) const {
    std::cout << "  " << suite.getSuiteName() << ": " << suite.getPassedCount() 
              << " passed, " << suite.getFailedCount() << " failed" << std::endl;
}

} // namespace CHTL