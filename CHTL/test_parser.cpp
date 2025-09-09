#include "CHTL/CHTLLexer/Lexer.h"
#include "CHTL/CHTLLexer/Token.h"
#include "CHTL/CHTLNode/BaseNode.h"
#include "CHTL/CHTLNode/RootNode.h"
#include "CHTL/CHTLNode/ElementNode.h"
#include "CHTL/CHTLNode/TextNode.h"
#include <iostream>
#include <fstream>
#include <string>
#include <memory>

class SimpleParser {
public:
    SimpleParser(const std::string& source) : lexer_(source) {}
    
    std::shared_ptr<CHTL::BaseNode> parse() {
        auto root = std::make_shared<CHTL::RootNode>();
        
        while (lexer_.hasMoreTokens()) {
            auto token = lexer_.nextToken();
            
            if (token->isType(CHTL::TokenType::END_OF_FILE)) {
                break;
            }
            
            if (token->isType(CHTL::TokenType::IDENTIFIER) || token->isType(CHTL::TokenType::LITERAL)) {
                auto element = parseElement(token->getValue());
                if (element) {
                    root->addChild(element);
                }
            }
        }
        
        return root;
    }
    
private:
    CHTL::Lexer lexer_;
    
    std::shared_ptr<CHTL::BaseNode> parseElement(const std::string& tagName) {
        auto element = std::make_shared<CHTL::ElementNode>(tagName);
        
        // 跳过空白和换行
        while (lexer_.hasMoreTokens()) {
            auto token = lexer_.peekToken();
            if (token->isType(CHTL::TokenType::LEFT_BRACE)) {
                break;
            }
            if (token->isType(CHTL::TokenType::END_OF_FILE)) {
                return element;
            }
            lexer_.nextToken();
        }
        
        if (lexer_.hasMoreTokens()) {
            auto token = lexer_.nextToken();
            if (token->isType(CHTL::TokenType::LEFT_BRACE)) {
                parseElementContent(element);
            }
        }
        
        return element;
    }
    
    void parseElementContent(std::shared_ptr<CHTL::BaseNode> element) {
        while (lexer_.hasMoreTokens()) {
            auto token = lexer_.peekToken();
            
            if (token->isType(CHTL::TokenType::RIGHT_BRACE)) {
                lexer_.nextToken();
                break;
            }
            
            if (token->isType(CHTL::TokenType::END_OF_FILE)) {
                break;
            }
            
            if (token->isType(CHTL::TokenType::IDENTIFIER) || token->isType(CHTL::TokenType::LITERAL)) {
                std::string tagName = token->getValue();
                lexer_.nextToken();
                
                if (tagName == "text") {
                    parseTextContent(element);
                } else {
                    auto childElement = parseElement(tagName);
                    if (childElement) {
                        element->addChild(childElement);
                    }
                }
            } else {
                lexer_.nextToken();
            }
        }
    }
    
    void parseTextContent(std::shared_ptr<CHTL::BaseNode> parent) {
        // 跳过空白和换行
        while (lexer_.hasMoreTokens()) {
            auto token = lexer_.peekToken();
            if (token->isType(CHTL::TokenType::LEFT_BRACE)) {
                break;
            }
            lexer_.nextToken();
        }
        
        if (lexer_.hasMoreTokens()) {
            auto token = lexer_.nextToken();
            if (token->isType(CHTL::TokenType::LEFT_BRACE)) {
                // 查找文本内容
                while (lexer_.hasMoreTokens()) {
                    auto contentToken = lexer_.peekToken();
                    if (contentToken->isType(CHTL::TokenType::RIGHT_BRACE)) {
                        lexer_.nextToken();
                        break;
                    }
                    if (contentToken->isType(CHTL::TokenType::STRING)) {
                        auto textNode = std::make_shared<CHTL::TextNode>(contentToken->getValue());
                        parent->addChild(textNode);
                    }
                    lexer_.nextToken();
                }
            }
        }
    }
};

void printAST(std::shared_ptr<CHTL::BaseNode> node, int depth = 0) {
    std::string indent(depth * 2, ' ');
    std::cout << indent << node->toString() << std::endl;
    
    for (const auto& child : node->getChildren()) {
        printAST(child, depth + 1);
    }
}

int main() {
    // 读取测试文件
    std::ifstream file("test.chtl");
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open test.chtl" << std::endl;
        return 1;
    }
    
    std::string source((std::istreambuf_iterator<char>(file)),
                      std::istreambuf_iterator<char>());
    file.close();
    
    std::cout << "Source code:" << std::endl;
    std::cout << source << std::endl;
    std::cout << "\n" << std::string(50, '=') << std::endl;
    
    // 解析CHTL代码
    SimpleParser parser(source);
    auto ast = parser.parse();
    
    std::cout << "AST:" << std::endl;
    printAST(ast);
    
    std::cout << "\n" << std::string(50, '=') << std::endl;
    std::cout << "Generated HTML:" << std::endl;
    std::cout << ast->toHTML() << std::endl;
    
    return 0;
}