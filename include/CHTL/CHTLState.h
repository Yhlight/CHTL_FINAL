#pragma once

#include <string>
#include <vector>
#include <unordered_map>

namespace CHTL {

/**
 * @brief CHTL编译状态
 * 管理编译过程中的状态信息
 */
class CHTLState {
public:
    CHTLState();
    ~CHTLState();
    
    // 位置信息
    void setPosition(size_t line, size_t column);
    size_t getCurrentLine() const;
    size_t getCurrentColumn() const;
    
    // 作用域管理
    void pushScope();
    void popScope();
    
    // 变量管理
    void addVariable(const std::string& name, const std::string& value);
    std::string getVariable(const std::string& name) const;
    bool hasVariable(const std::string& name) const;
    
    // 清理
    void clear();

private:
    size_t m_currentLine;
    size_t m_currentColumn;
    std::vector<std::unordered_map<std::string, std::string>> m_scopeStack;
};

} // namespace CHTL