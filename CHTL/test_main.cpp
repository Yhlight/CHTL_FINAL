#include <iostream>
#include <fstream>
#include "CHTL/CHTLLexer/CHTLLexer.h"
#include "CHTL/CHTLLexer/Token.h"
#include "Scanner/CHTLUnifiedScanner.h"

int main() {
    std::cout << "CHTL基础功能测试" << std::endl;
    std::cout << "=================" << std::endl;
    
    try {
        // 读取测试文件
        std::ifstream file("test.chtl");
        if (!file.is_open()) {
            std::cerr << "无法打开测试文件" << std::endl;
            return 1;
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string source = buffer.str();
        file.close();
        
        std::cout << "源代码:" << std::endl;
        std::cout << source << std::endl;
        std::cout << std::endl;
        
        // 测试词法分析器
        std::cout << "词法分析结果:" << std::endl;
        CHTL::CHTLLexer lexer(source);
        auto tokens = lexer.tokenize();
        
        for (size_t i = 0; i < tokens.size() && i < 20; ++i) {
            auto token = tokens.peek(i);
            if (token.getTypeName() != "END_OF_FILE") {
                std::cout << token.toString() << std::endl;
            }
        }
        std::cout << std::endl;
        
        // 测试统一扫描器
        std::cout << "统一扫描结果:" << std::endl;
        CHTL::CHTLUnifiedScanner scanner(source);
        auto scanResult = scanner.scan();
        
        std::cout << "扫描到 " << scanResult.fragments.size() << " 个代码片段:" << std::endl;
        for (const auto& fragment : scanResult.fragments) {
            std::cout << "类型: " << fragment.type 
                      << ", 行: " << fragment.startLine << "-" << fragment.endLine
                      << ", 长度: " << fragment.content.length() << std::endl;
        }
        
        if (!scanResult.errors.empty()) {
            std::cout << "错误:" << std::endl;
            for (const auto& error : scanResult.errors) {
                std::cout << "  " << error << std::endl;
            }
        }
        
        if (!scanResult.warnings.empty()) {
            std::cout << "警告:" << std::endl;
            for (const auto& warning : scanResult.warnings) {
                std::cout << "  " << warning << std::endl;
            }
        }
        
    } catch (const std::exception& e) {
        std::cerr << "测试过程中发生错误: " << e.what() << std::endl;
        return 1;
    }
    
    std::cout << std::endl << "测试完成!" << std::endl;
    return 0;
}