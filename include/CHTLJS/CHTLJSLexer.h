#pragma once

#include <string>
#include <vector>

namespace CHTL {

/**
 * @brief CHTL JS词法分析器
 * 负责分析CHTL JS代码的词法单元
 */
class CHTLJSLexer {
public:
    CHTLJSLexer();
    ~CHTLJSLexer();
    
    // 词法分析
    std::vector<std::string> tokenize(const std::string& code);

private:
    // 辅助方法
    bool isSpecialCharacter(char c) const;
};

} // namespace CHTL