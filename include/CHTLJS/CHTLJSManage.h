#pragma once

#include <string>
#include <unordered_map>

namespace CHTL {

/**
 * @brief CHTL JS管理器
 * 负责管理CHTL JS函数和组件
 */
class CHTLJSManage {
public:
    CHTLJSManage();
    ~CHTLJSManage();
    
    // 函数管理
    void addFunction(const std::string& name, const std::string& definition);
    std::string getFunction(const std::string& name) const;
    bool hasFunction(const std::string& name) const;

private:
    std::unordered_map<std::string, std::string> m_functions;
};

} // namespace CHTL