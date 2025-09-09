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
#include <stack>

class CHTLDemo {
public:
    CHTLDemo(const std::string& source) : lexer_(source) {}
    
    void run() {
        std::cout << "=== CHTL Compiler Demo ===" << std::endl;
        std::cout << "\n1. Source Code:" << std::endl;
        std::cout << "----------------" << std::endl;
        printSource();
        
        std::cout << "\n2. Lexical Analysis:" << std::endl;
        std::cout << "-------------------" << std::endl;
        performLexicalAnalysis();
        
        std::cout << "\n3. Syntax Analysis (AST):" << std::endl;
        std::cout << "------------------------" << std::endl;
        auto ast = performSyntaxAnalysis();
        
        std::cout << "\n4. Code Generation:" << std::endl;
        std::cout << "------------------" << std::endl;
        generateCode(ast);
        
        std::cout << "\n=== Demo Complete ===" << std::endl;
    }
    
private:
    CHTL::Lexer lexer_;
    std::string source_;
    
    void printSource() {
        // 重新创建 lexer 来获取源码
        CHTL::Lexer tempLexer(source_);
        // 这里我们直接显示源码，实际应该从文件读取
        std::ifstream file("test.chtl");
        if (file.is_open()) {
            std::string line;
            while (std::getline(file, line)) {
                std::cout << line << std::endl;
            }
            file.close();
        }
    }
    
    void performLexicalAnalysis() {
        CHTL::Lexer tempLexer(source_);
        int tokenCount = 0;
        
        while (tempLexer.hasMoreTokens()) {
            auto token = tempLexer.nextToken();
            if (token->isType(CHTL::TokenType::END_OF_FILE)) {
                break;
            }
            std::cout << token->toString() << std::endl;
            tokenCount++;
        }
        
        std::cout << "\nTotal tokens: " << tokenCount << std::endl;
    }
    
    std::shared_ptr<CHTL::BaseNode> performSyntaxAnalysis() {
        CHTL::Lexer tempLexer(source_);
        auto root = std::make_shared<CHTL::RootNode>();
        
        // 简化的解析逻辑
        std::stack<std::shared_ptr<CHTL::BaseNode>> nodeStack;
        nodeStack.push(root);
        
        while (tempLexer.hasMoreTokens()) {
            auto token = tempLexer.nextToken();
            
            if (token->isType(CHTL::TokenType::END_OF_FILE)) {
                break;
            }
            
            if (token->isType(CHTL::TokenType::IDENTIFIER) || token->isType(CHTL::TokenType::LITERAL)) {
                if (token->getValue() == "text") {
                    // 处理文本节点
                    tempLexer.nextToken(); // 跳过 {
                    auto textToken = tempLexer.nextToken();
                    if (textToken->isType(CHTL::TokenType::STRING)) {
                        auto textNode = std::make_shared<CHTL::TextNode>(textToken->getValue());
                        nodeStack.top()->addChild(textNode);
                    }
                    tempLexer.nextToken(); // 跳过 }
                } else {
                    // 处理元素节点
                    auto element = std::make_shared<CHTL::ElementNode>(token->getValue());
                    nodeStack.top()->addChild(element);
                    
                    // 检查是否有子内容
                    auto nextToken = tempLexer.peekToken();
                    if (nextToken && nextToken->isType(CHTL::TokenType::LEFT_BRACE)) {
                        tempLexer.nextToken(); // 跳过 {
                        nodeStack.push(element);
                    }
                }
            } else if (token->isType(CHTL::TokenType::RIGHT_BRACE)) {
                if (!nodeStack.empty()) {
                    nodeStack.pop();
                }
            }
        }
        
        printAST(root);
        return root;
    }
    
    void printAST(std::shared_ptr<CHTL::BaseNode> node, int depth = 0) {
        std::string indent(depth * 2, ' ');
        std::cout << indent << node->toString() << std::endl;
        
        for (const auto& child : node->getChildren()) {
            printAST(child, depth + 1);
        }
    }
    
    void generateCode(std::shared_ptr<CHTL::BaseNode> ast) {
        std::cout << "Generated HTML:" << std::endl;
        std::cout << "---------------" << std::endl;
        std::cout << ast->toHTML() << std::endl;
        
        std::cout << "\nGenerated CSS:" << std::endl;
        std::cout << "--------------" << std::endl;
        std::cout << ast->toCSS() << std::endl;
        
        std::cout << "\nGenerated JavaScript:" << std::endl;
        std::cout << "-------------------" << std::endl;
        std::cout << ast->toJS() << std::endl;
    }
};

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
    
    // 运行演示
    CHTLDemo demo(source);
    demo.run();
    
    return 0;
}