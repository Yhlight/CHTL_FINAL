#include <iostream>
#include "CHTL/CHTLLexer/CHTLLexer.h"
#include "CHTL/CHTLParser/CHTLParser.h"

// 添加调试信息到解析器
class DebugCHTLParser : public CHTL::CHTLParser {
public:
    void debugParse(const std::string& source) {
        std::cout << "=== 调试解析器 ===" << std::endl;
        
        CHTL::CHTLLexer lexer;
        auto tokens = lexer.tokenize(source);
        
        std::cout << "Tokens:" << std::endl;
        for (size_t i = 0; i < tokens.size(); i++) {
            std::cout << "  " << i << ": " << tokens[i].toString() << std::endl;
        }
        
        setTokens(tokens);
        
        std::cout << "\n开始解析..." << std::endl;
        
        // 手动模拟解析过程
        size_t step = 0;
        while (getCurrentTokenIndex() < tokens.size() && step < 20) {
            std::cout << "Step " << step << ": 当前token[" << getCurrentTokenIndex() << "] = " 
                      << getCurrentToken().toString() << std::endl;
            
            if (getCurrentToken().type == CHTL::CHTLTokenType::EOF_TOKEN) {
                std::cout << "到达EOF，停止解析" << std::endl;
                break;
            }
            
            step++;
            advanceDebug();
        }
        
        if (step >= 20) {
            std::cout << "解析步骤超过20次，可能存在无限循环" << std::endl;
        }
    }
    
private:
    size_t getCurrentTokenIndex() const {
        return currentToken;
    }
    
    CHTL::CHTLToken getCurrentToken() const {
        return current();
    }
    
    void advanceDebug() {
        if (currentToken < tokens.size() - 1) {
            currentToken++;
        }
    }
};

int main() {
    std::string source = "div\n{\n    id: test;\n}";
    
    DebugCHTLParser parser;
    parser.debugParse(source);
    
    return 0;
}