#include "CHTL/CHTLLexer/CHTLLexer.h"
#include "CHTL/CHTLParser/CHTLParser.h"
#include <iostream>
#include <string>

int main() {
    std::cout << "=== 逐步模板调试 ===" << std::endl;
    
    // 最简单的模板测试
    std::string source = "[Template] @Style DefaultText { color: black; }";
    std::cout << "源代码: " << source << std::endl;
    
    // 词法分析
    CHTL::CHTLLexer lexer(source);
    auto tokens = lexer.tokenize();
    
    std::cout << "\nTokens:" << std::endl;
    for (size_t i = 0; i < tokens.size(); ++i) {
        std::cout << "  " << i << ": " << tokens[i].toString() << std::endl;
    }
    
    // 语法分析
    std::cout << "\n开始语法分析..." << std::endl;
    CHTL::CHTLParser parser;
    parser.setTokens(tokens);
    
    // 手动模拟解析过程
    std::cout << "\n手动模拟解析过程:" << std::endl;
    
    // 模拟解析器的状态
    size_t currentToken = 0;
    auto current = [&]() -> const CHTL::CHTLToken& {
        if (currentToken < tokens.size()) {
            return tokens[currentToken];
        }
        return tokens.back(); // EOF token
    };
    
    auto advance = [&]() {
        if (currentToken < tokens.size()) {
            currentToken++;
        }
    };
    
    auto match = [&](CHTL::CHTLTokenType type) -> bool {
        return current().type == type;
    };
    
    // 解析 [Template]
    std::cout << "步骤1: 解析 [Template]" << std::endl;
    std::cout << "  当前token: " << current().toString() << std::endl;
    if (match(CHTL::CHTLTokenType::TEMPLATE)) {
        std::cout << "  匹配TEMPLATE，前进" << std::endl;
        advance();
    } else {
        std::cout << "  错误：期望TEMPLATE" << std::endl;
        return 1;
    }
    
    // 解析 @Style
    std::cout << "步骤2: 解析 @Style" << std::endl;
    std::cout << "  当前token: " << current().toString() << std::endl;
    if (match(CHTL::CHTLTokenType::TEMPLATE_STYLE)) {
        std::cout << "  匹配TEMPLATE_STYLE，前进" << std::endl;
        advance();
    } else {
        std::cout << "  错误：期望TEMPLATE_STYLE" << std::endl;
        return 1;
    }
    
    // 解析 DefaultText
    std::cout << "步骤3: 解析 DefaultText" << std::endl;
    std::cout << "  当前token: " << current().toString() << std::endl;
    if (match(CHTL::CHTLTokenType::IDENTIFIER)) {
        std::cout << "  匹配IDENTIFIER，前进" << std::endl;
        advance();
    } else {
        std::cout << "  错误：期望IDENTIFIER" << std::endl;
        return 1;
    }
    
    // 解析 {
    std::cout << "步骤4: 解析 {" << std::endl;
    std::cout << "  当前token: " << current().toString() << std::endl;
    if (match(CHTL::CHTLTokenType::LEFT_BRACE)) {
        std::cout << "  匹配LEFT_BRACE，前进" << std::endl;
        advance();
    } else {
        std::cout << "  错误：期望LEFT_BRACE" << std::endl;
        return 1;
    }
    
    // 解析 color: black;
    std::cout << "步骤5: 解析 color: black;" << std::endl;
    std::cout << "  当前token: " << current().toString() << std::endl;
    if (match(CHTL::CHTLTokenType::IDENTIFIER)) {
        std::string propName = current().value;
        std::cout << "  属性名: " << propName << std::endl;
        advance();
        
        std::cout << "  当前token: " << current().toString() << std::endl;
        if (match(CHTL::CHTLTokenType::COLON)) {
            std::cout << "  匹配COLON，前进" << std::endl;
            advance();
            
            std::cout << "  当前token: " << current().toString() << std::endl;
            if (match(CHTL::CHTLTokenType::IDENTIFIER)) {
                std::string propValue = current().value;
                std::cout << "  属性值: " << propValue << std::endl;
                advance();
                
                std::cout << "  当前token: " << current().toString() << std::endl;
                if (match(CHTL::CHTLTokenType::SEMICOLON)) {
                    std::cout << "  匹配SEMICOLON，前进" << std::endl;
                    advance();
                } else {
                    std::cout << "  错误：期望SEMICOLON，但得到: " << current().toString() << std::endl;
                    return 1;
                }
            } else {
                std::cout << "  错误：期望IDENTIFIER，但得到: " << current().toString() << std::endl;
                return 1;
            }
        } else {
            std::cout << "  错误：期望COLON，但得到: " << current().toString() << std::endl;
            return 1;
        }
    } else {
        std::cout << "  错误：期望IDENTIFIER，但得到: " << current().toString() << std::endl;
        return 1;
    }
    
    // 解析 }
    std::cout << "步骤6: 解析 }" << std::endl;
    std::cout << "  当前token: " << current().toString() << std::endl;
    if (match(CHTL::CHTLTokenType::RIGHT_BRACE)) {
        std::cout << "  匹配RIGHT_BRACE，前进" << std::endl;
        advance();
    } else {
        std::cout << "  错误：期望RIGHT_BRACE，但得到: " << current().toString() << std::endl;
        return 1;
    }
    
    std::cout << "\n最终token: " << current().toString() << std::endl;
    std::cout << "解析成功！" << std::endl;
    
    return 0;
}