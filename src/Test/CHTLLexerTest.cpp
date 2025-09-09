#include "Test/CHTLTest.h"
#include "CHTL/CHTLLexer.h"
#include <functional>
#include <iostream>

namespace CHTL {

class CHTLLexerTest {
public:
    static void runTests() {
        CHTLTestSuite suite("CHTL Lexer Tests");
        
        // 基本词法分析测试
        suite.addTest("Basic Tokenization", []() {
            std::string code = "html { div { class: test; } }";
            CHTLLexer lexer(code);
            auto tokens = lexer.tokenize();
            
            return TestAssert::assertTrue(tokens.size() > 0, "Should generate tokens") &&
                   TestAssert::assertEquals("html", tokens[0].value, "First token should be 'html'");
        });
        
        // 字符串处理测试
        suite.addTest("String Handling", []() {
            std::string code = "\"Hello, World!\"";
            CHTLLexer lexer(code);
            auto tokens = lexer.tokenize();
            
            return TestAssert::assertTrue(tokens.size() > 0, "Should generate tokens") &&
                   TestAssert::assertEquals("Hello, World!", tokens[0].value, "Should handle strings correctly");
        });
        
        // 注释处理测试
        suite.addTest("Comment Handling", []() {
            std::string code = "// This is a comment\nhtml { }";
            CHTLLexer lexer(code);
            auto tokens = lexer.tokenize();
            
            // 应该跳过注释，只处理html和{}
            return TestAssert::assertTrue(tokens.size() >= 3, "Should skip comments") &&
                   TestAssert::assertEquals("html", tokens[0].value, "Should find html token");
        });
        
        // 属性处理测试
        suite.addTest("Attribute Handling", []() {
            std::string code = "div { class: test; id: main; }";
            CHTLLexer lexer(code);
            auto tokens = lexer.tokenize();
            
            return TestAssert::assertTrue(tokens.size() > 0, "Should generate tokens") &&
                   TestAssert::assertEquals("div", tokens[0].value, "Should find div token");
        });
        
        // 嵌套结构测试
        suite.addTest("Nested Structure", []() {
            std::string code = "html { head { title { \"Test\" } } }";
            CHTLLexer lexer(code);
            auto tokens = lexer.tokenize();
            
            return TestAssert::assertTrue(tokens.size() > 0, "Should generate tokens") &&
                   TestAssert::assertEquals("html", tokens[0].value, "Should find html token");
        });
        
        // 错误处理测试
        suite.addTest("Error Handling", []() {
            std::string code = "html { div { class: test; }"; // 缺少闭合括号
            CHTLLexer lexer(code);
            auto tokens = lexer.tokenize();
            
            // 即使有语法错误，词法分析器也应该能处理
            return TestAssert::assertTrue(tokens.size() > 0, "Should still generate tokens");
        });
        
        suite.run();
    }
};

} // namespace CHTL