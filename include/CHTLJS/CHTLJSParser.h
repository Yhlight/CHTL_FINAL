#pragma once

#include <string>
#include <vector>

namespace CHTL {

/**
 * @brief CHTL JS解析器
 * 负责解析CHTL JS代码
 */
class CHTLJSParser {
public:
    CHTLJSParser();
    ~CHTLJSParser();
    
    // 代码解析
    std::string parse(const std::string& code);

private:
    // 解析方法
    std::string parseFileLoader(const std::vector<std::string>& tokens, size_t& index);
    std::string parseScriptBlock(const std::vector<std::string>& tokens, size_t& index);
    std::string parseListener(const std::vector<std::string>& tokens, size_t& index);
    std::string parseDelegate(const std::vector<std::string>& tokens, size_t& index);
    std::string parseAnimation(const std::vector<std::string>& tokens, size_t& index);
    std::string parseVirtualObject(const std::vector<std::string>& tokens, size_t& index);
    std::string parseRouter(const std::vector<std::string>& tokens, size_t& index);
    std::string parseSelector(const std::string& token);
};

} // namespace CHTL