#include "Test/CHTLTest.h"
#include "CHTL/CHTLParser.h"
#include "CHTL/CHTLLexer.h"
#include "CHTL/CHTLNode/ElementNode.h"
#include "CHTL/CHTLNode/TextNode.h"
#include "CHTL/CHTLNode/CommentNode.h"
#include <functional>
#include <memory>
#include <iostream>

namespace CHTL {

class CHTLParserTest {
public:
    static void runTests() {
        CHTLTestSuite suite("CHTL Parser Tests");
        
        // 基本解析测试
        suite.addTest("Basic Parsing", []() -> bool {
            std::string code = "html { div { \"Hello\" } }";
            CHTLLexer lexer(code);
            auto tokens = lexer.tokenize();
            
            auto lexerPtr = std::make_shared<CHTLLexer>(code);
            CHTLParser parser(lexerPtr);
            auto ast = parser.parseDocument();
            
            return TestAssert::assertTrue(ast != nullptr, "Should create AST") &&
                   TestAssert::assertEquals("html", ast->getTagName(), "Root should be html");
        });
        
        // 元素解析测试
        suite.addTest("Element Parsing", []() -> bool {
            std::string code = "div { class: test; id: main; }";
            auto lexerPtr = std::make_shared<CHTLLexer>(code);
            CHTLParser parser(lexerPtr);
            auto ast = parser.parseElement();
            
            return TestAssert::assertTrue(ast != nullptr, "Should create element") &&
                   TestAssert::assertEquals("div", ast->getTagName(), "Should be div element");
        });
        
        // 文本解析测试
        suite.addTest("Text Parsing", []() -> bool {
            std::string code = "\"Hello, World!\"";
            auto lexerPtr = std::make_shared<CHTLLexer>(code);
            CHTLParser parser(lexerPtr);
            auto ast = parser.parseText();
            
            return TestAssert::assertTrue(ast != nullptr, "Should create text node") &&
                   TestAssert::assertEquals("Hello, World!", ast->getText(), "Should contain correct text");
        });
        
        // 注释解析测试
        suite.addTest("Comment Parsing", []() -> bool {
            std::string code = "// This is a comment";
            auto lexerPtr = std::make_shared<CHTLLexer>(code);
            CHTLParser parser(lexerPtr);
            auto ast = parser.parseComment();
            
            return TestAssert::assertTrue(ast != nullptr, "Should create comment node");
        });
        
        // 属性解析测试
        suite.addTest("Attribute Parsing", []() -> bool {
            std::string code = "div { class: test; id: main; }";
            auto lexerPtr = std::make_shared<CHTLLexer>(code);
            CHTLParser parser(lexerPtr);
            auto ast = parser.parseElement();
            
            return TestAssert::assertTrue(ast != nullptr, "Should create element") &&
                   TestAssert::assertTrue(ast->hasAttribute("class"), "Should have class attribute") &&
                   TestAssert::assertEquals("test", ast->getAttribute("class"), "Class should be 'test'") &&
                   TestAssert::assertTrue(ast->hasAttribute("id"), "Should have id attribute") &&
                   TestAssert::assertEquals("main", ast->getAttribute("id"), "ID should be 'main'");
        });
        
        // 嵌套结构解析测试
        suite.addTest("Nested Structure Parsing", []() -> bool {
            std::string code = "html { head { title { \"Test\" } } }";
            auto lexerPtr = std::make_shared<CHTLLexer>(code);
            CHTLParser parser(lexerPtr);
            auto ast = parser.parseDocument();
            
            return TestAssert::assertTrue(ast != nullptr, "Should create AST") &&
                   TestAssert::assertEquals("html", ast->getTagName(), "Root should be html") &&
                   TestAssert::assertTrue(ast->getChildren().size() > 0, "Should have children");
        });
        
        // HTML元素识别测试
        suite.addTest("HTML Element Recognition", []() {
            CHTLParser parser;
            
            return TestAssert::assertTrue(parser.isHTMLElement("div"), "div should be HTML element") &&
                   TestAssert::assertTrue(parser.isHTMLElement("span"), "span should be HTML element") &&
                   TestAssert::assertTrue(parser.isHTMLElement("html"), "html should be HTML element") &&
                   TestAssert::assertFalse(parser.isHTMLElement("custom"), "custom should not be HTML element");
        });
        
        // 错误处理测试
        suite.addTest("Error Handling", []() {
            std::string code = "html { div { class: test; }"; // 缺少闭合括号
            CHTLLexer lexer(code);
            auto tokens = lexer.tokenize();
            
            CHTLParser parser(tokens);
            auto ast = parser.parseDocument();
            
            // 即使有语法错误，解析器也应该能处理
            return TestAssert::assertTrue(ast != nullptr, "Should still create AST");
        });
        
        suite.run();
    }
};

} // namespace CHTL