#pragma once

#include <string>
#include <unordered_map>

namespace CHTL {

/**
 * @brief CHTL JS上下文
 * 管理CHTL JS编译过程中的状态
 */
class CHTLJSContext {
public:
    CHTLJSContext();
    ~CHTLJSContext();
    
    // 函数管理
    void addFunction(const std::string& name, const std::string& definition);
    std::string getFunction(const std::string& name) const;
    bool hasFunction(const std::string& name) const;

private:
    std::unordered_map<std::string, std::string> m_functions;
};

} // namespace CHTL