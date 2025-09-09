#include <iostream>
#include <fstream>
#include "CHTL/CHTLLexer/CHTLLexer.h"
#include "CHTL/CHTLLexer/Token.h"
#include "CHTL/CHTLParser/CHTLParser.h"
#include "CHTL/CHTLGenerator/CHTLGenerator.h"
#include "Scanner/CHTLUnifiedScanner.h"
#include "CompilerDispatcher/CompilerDispatcher.h"

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
        
        // 测试语法分析器
        std::cout << "语法分析结果:" << std::endl;
        CHTL::CHTLParser parser(tokens);
        auto ast = parser.parse();
        
        if (ast) {
            std::cout << "AST创建成功，节点数: " << ast->getChildCount() << std::endl;
            
            // 显示解析错误
            auto errors = parser.getErrors();
            if (!errors.empty()) {
                std::cout << "解析错误:" << std::endl;
                for (const auto& error : errors) {
                    std::cout << "  " << error << std::endl;
                }
            }
        } else {
            std::cout << "AST创建失败" << std::endl;
        }
        
        // 测试代码生成器
        if (ast) {
            std::cout << "代码生成结果:" << std::endl;
            CHTL::CHTLGenerator generator;
            generator.setDebugMode(true);
            std::string html = generator.generate(ast);
            
            std::cout << "生成的HTML长度: " << html.length() << std::endl;
            std::cout << "HTML内容:" << std::endl;
            std::cout << html << std::endl;
            
            // 显示生成器错误
            auto errors = generator.getErrors();
            if (!errors.empty()) {
                std::cout << "生成错误:" << std::endl;
                for (const auto& error : errors) {
                    std::cout << "  " << error << std::endl;
                }
            }
        }
        
        // 测试完整编译流程
        std::cout << "完整编译流程测试:" << std::endl;
        CHTL::CompilerDispatcher dispatcher;
        dispatcher.setDebugMode(true);
        auto result = dispatcher.compileFile("test.chtl");
        
        std::cout << "编译结果HTML长度: " << result.html.length() << std::endl;
        std::cout << "编译结果CSS长度: " << result.css.length() << std::endl;
        std::cout << "编译结果JavaScript长度: " << result.javascript.length() << std::endl;
        
        if (!result.errors.empty()) {
            std::cout << "编译错误:" << std::endl;
            for (const auto& error : result.errors) {
                std::cout << "  " << error << std::endl;
            }
        }
        
        if (!result.warnings.empty()) {
            std::cout << "编译警告:" << std::endl;
            for (const auto& warning : result.warnings) {
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