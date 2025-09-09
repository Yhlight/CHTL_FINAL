#include "Test/CHTLTest.h"
#include "CHTL/CHTLGenerator.h"
#include "CHTL/CHTLNode/ElementNode.h"
#include "CHTL/CHTLNode/TextNode.h"
#include "CHTL/CHTLNode/CommentNode.h"
#include <iostream>

namespace CHTL {

class CHTLGeneratorTest {
public:
    static void runTests() {
        CHTLTestSuite suite("CHTL Generator Tests");
        
        // 基本HTML生成测试
        suite.addTest("Basic HTML Generation", []() {
            auto element = std::make_shared<ElementNode>("div");
            element->setAttribute("class", "test");
            element->setAttribute("id", "main");
            
            CHTLGenerator generator;
            std::string html = generator.generateHTML(element);
            
            return TestAssert::assertContains(html, "<div", "Should contain div tag") &&
                   TestAssert::assertContains(html, "class=\"test\"", "Should contain class attribute") &&
                   TestAssert::assertContains(html, "id=\"main\"", "Should contain id attribute") &&
                   TestAssert::assertContains(html, "</div>", "Should contain closing tag");
        });
        
        // 文本节点生成测试
        suite.addTest("Text Node Generation", []() {
            auto text = std::make_shared<TextNode>("Hello, World!");
            
            CHTLGenerator generator;
            std::string html = generator.generateHTML(text);
            
            return TestAssert::assertContains(html, "Hello, World!", "Should contain text content");
        });
        
        // 注释生成测试
        suite.addTest("Comment Generation", []() {
            auto comment = std::make_shared<CommentNode>(CommentType::LINE, "This is a comment");
            
            CHTLGenerator generator;
            std::string html = generator.generateHTML(comment);
            
            return TestAssert::assertContains(html, "<!--", "Should contain comment start") &&
                   TestAssert::assertContains(html, "This is a comment", "Should contain comment content") &&
                   TestAssert::assertContains(html, "-->", "Should contain comment end");
        });
        
        // 嵌套结构生成测试
        suite.addTest("Nested Structure Generation", []() {
            auto html = std::make_shared<ElementNode>("html");
            auto head = std::make_shared<ElementNode>("head");
            auto title = std::make_shared<ElementNode>("title");
            auto titleText = std::make_shared<TextNode>("Test Page");
            
            title->addChild(titleText);
            head->addChild(title);
            html->addChild(head);
            
            CHTLGenerator generator;
            std::string result = generator.generateHTML(html);
            
            return TestAssert::assertContains(result, "<html>", "Should contain html tag") &&
                   TestAssert::assertContains(result, "<head>", "Should contain head tag") &&
                   TestAssert::assertContains(result, "<title>", "Should contain title tag") &&
                   TestAssert::assertContains(result, "Test Page", "Should contain title text") &&
                   TestAssert::assertContains(result, "</title>", "Should contain closing title tag") &&
                   TestAssert::assertContains(result, "</head>", "Should contain closing head tag") &&
                   TestAssert::assertContains(result, "</html>", "Should contain closing html tag");
        });
        
        // 自闭合标签生成测试
        suite.addTest("Self-Closing Tag Generation", []() {
            auto img = std::make_shared<ElementNode>("img");
            img->setAttribute("src", "test.jpg");
            img->setAttribute("alt", "Test Image");
            img->setSelfClosing(true);
            
            CHTLGenerator generator;
            std::string html = generator.generateHTML(img);
            
            return TestAssert::assertContains(html, "<img", "Should contain img tag") &&
                   TestAssert::assertContains(html, "src=\"test.jpg\"", "Should contain src attribute") &&
                   TestAssert::assertContains(html, "alt=\"Test Image\"", "Should contain alt attribute") &&
                   TestAssert::assertContains(html, "/>", "Should be self-closing");
        });
        
        // CSS生成测试
        suite.addTest("CSS Generation", []() {
            auto style = std::make_shared<StyleNode>();
            style->addSelector(".test");
            style->addProperty("color", "red");
            style->addProperty("font-size", "16px");
            
            CHTLGenerator generator;
            std::string css = generator.generateCSS(style);
            
            return TestAssert::assertContains(css, ".test", "Should contain selector") &&
                   TestAssert::assertContains(css, "color: red", "Should contain color property") &&
                   TestAssert::assertContains(css, "font-size: 16px", "Should contain font-size property");
        });
        
        // JavaScript生成测试
        suite.addTest("JavaScript Generation", []() {
            auto script = std::make_shared<ScriptNode>();
            script->addCode("console.log('Hello, World!');");
            
            CHTLGenerator generator;
            std::string js = generator.generateJavaScript(script);
            
            return TestAssert::assertContains(js, "console.log", "Should contain console.log") &&
                   TestAssert::assertContains(js, "Hello, World!", "Should contain message");
        });
        
        // HTML转义测试
        suite.addTest("HTML Escaping", []() {
            auto text = std::make_shared<TextNode>("<script>alert('XSS')</script>");
            
            CHTLGenerator generator;
            std::string html = generator.generateHTML(text);
            
            return TestAssert::assertContains(html, "&lt;script&gt;", "Should escape <script>") &&
                   TestAssert::assertContains(html, "&lt;/script&gt;", "Should escape </script>");
        });
        
        // 格式化测试
        suite.addTest("Pretty Printing", []() {
            auto html = std::make_shared<ElementNode>("html");
            auto body = std::make_shared<ElementNode>("body");
            auto div = std::make_shared<ElementNode>("div");
            auto text = std::make_shared<TextNode>("Hello");
            
            div->addChild(text);
            body->addChild(div);
            html->addChild(body);
            
            CHTLGenerator generator;
            generator.setPrettyPrint(true);
            std::string result = generator.generateHTML(html);
            
            return TestAssert::assertContains(result, "\n", "Should contain newlines for formatting");
        });
        
        suite.run();
    }
};

} // namespace CHTL