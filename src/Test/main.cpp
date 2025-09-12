#include <iostream>
#include <fstream>
#include <string>
#include "CHTL/CHTLLexer.h"
#include "CHTL/CHTLParser.h"
#include "CHTL/CHTLGenerator.h"
#include "Scanner/UnifiedScanner.h"

using namespace CHTL;

void testLexer() {
    std::cout << "Testing CHTL Lexer..." << std::endl;
    
    CHTLLexer lexer;
    lexer.setSource("text { Hello World }");
    
    auto tokens = lexer.tokenize();
    
    for (const auto& token : tokens) {
        std::cout << token.toString() << std::endl;
    }
    
    std::cout << "Lexer test completed." << std::endl << std::endl;
}

void testScanner() {
    std::cout << "Testing Unified Scanner..." << std::endl;
    
    std::string sourceCode = R"(
        html {
            head {
                title { text { Test } }
            }
            body {
                div {
                    style {
                        color: red;
                    }
                }
            }
        }
    )";
    
    UnifiedScanner scanner;
    auto fragments = scanner.scan(sourceCode);
    
    std::cout << "Found " << fragments.size() << " code fragments:" << std::endl;
    for (const auto& fragment : fragments) {
        std::cout << "Type: " << static_cast<int>(fragment.type) 
                  << ", Content: " << fragment.content.substr(0, 50) << "..." << std::endl;
    }
    
    std::cout << "Scanner test completed." << std::endl << std::endl;
}

void testGenerator() {
    std::cout << "Testing CHTL Generator..." << std::endl;
    
    // 创建简单的AST
    auto root = std::make_unique<ASTNode>(ASTNode::NodeType::ROOT);
    auto html = std::make_unique<ElementNode>("html");
    auto body = std::make_unique<ElementNode>("body");
    auto div = std::make_unique<ElementNode>("div");
    auto text = std::make_unique<TextNode>("Hello CHTL!");
    
    div->addChild(std::move(text));
    body->addChild(std::move(div));
    html->addChild(std::move(body));
    root->addChild(std::move(html));
    
    CHTLGenerator generator;
    std::string htmlOutput = generator.generateHTML(root.get());
    
    std::cout << "Generated HTML:" << std::endl;
    std::cout << htmlOutput << std::endl;
    
    std::cout << "Generator test completed." << std::endl << std::endl;
}

int main() {
    std::cout << "CHTL Test Suite" << std::endl;
    std::cout << "===============" << std::endl << std::endl;
    
    testLexer();
    testScanner();
    testGenerator();
    
    std::cout << "All tests completed!" << std::endl;
    
    return 0;
}