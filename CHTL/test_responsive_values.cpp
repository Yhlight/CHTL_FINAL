#include <iostream>
#include <string>
#include "CHTL_JS/CHTLJSLexer/CHTLJSLexer.h"
#include "CHTL_JS/CHTLJSLexer/CHTLJSToken.h"
#include "CHTL_JS/CHTLJSParser/CHTLJSParser.h"
#include "CHTL_JS/CHTLJSGenerator/CHTLJSGenerator.h"

using namespace CHTL_JS;

int main() {
    std::cout << "=== CHTL JS 响应式值功能测试 ===" << std::endl;
    
    // 测试代码
    std::string testCode = R"(
        let boxClass = "box";
        let boxWidth = 100;
        
        // 测试响应式值
        class = $boxClass$;
        width: $boxWidth$ + 20 + "px";
        
        // 测试响应式值在表达式中的使用
        let result = $boxWidth$ * 2;
    )";
    
    std::cout << "测试代码:" << std::endl;
    std::cout << testCode << std::endl;
    std::cout << std::endl;
    
    // 词法分析
    CHTLJSLexer lexer;
    lexer.setSource(testCode);
    auto tokens = lexer.tokenize();
    
    std::cout << "词法分析结果:" << std::endl;
    for (const auto& token : tokens) {
        if (token.getType() == CHTLJSTokenType::RESPONSIVE_VALUE) {
            std::cout << "  响应式值: " << token.getValue() << " (类型: " << token.getTypeName() << ")" << std::endl;
        }
    }
    std::cout << std::endl;
    
    // 语法分析
    CHTLJSParser parser;
    parser.setTokens(tokens);
    auto ast = parser.parse();
    
    if (ast) {
        std::cout << "语法分析成功!" << std::endl;
        std::cout << "AST节点类型: " << ast->getNodeTypeName() << std::endl;
        std::cout << "子节点数量: " << ast->getChildCount() << std::endl;
    } else {
        std::cout << "语法分析失败!" << std::endl;
    }
    std::cout << std::endl;
    
    // 代码生成
    CHTLJSGenerator generator;
    std::string generatedCode = generator.generate(ast);
    
    std::cout << "生成的JavaScript代码:" << std::endl;
    std::cout << generatedCode << std::endl;
    std::cout << std::endl;
    
    // 检查是否有错误
    if (generator.hasErrors()) {
        std::cout << "生成器错误:" << std::endl;
        for (const auto& error : generator.getErrors()) {
            std::cout << "  " << error << std::endl;
        }
    } else {
        std::cout << "代码生成成功，无错误!" << std::endl;
    }
    
    return 0;
}