#pragma once

#include <string>

namespace CHTL {

/**
 * @brief CHTL JS状态
 * 管理CHTL JS编译过程中的状态
 */
class CHTLJSState {
public:
    CHTLJSState();
    ~CHTLJSState();
    
    // 当前函数
    void setCurrentFunction(const std::string& functionName);
    std::string getCurrentFunction() const;

private:
    std::string m_currentFunction;
};

} // namespace CHTL