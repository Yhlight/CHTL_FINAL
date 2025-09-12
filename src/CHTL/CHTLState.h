#ifndef CHTL_STATE_H
#define CHTL_STATE_H

#include <string>
#include <map>
#include <vector>
#include <stack>

namespace CHTL {

/**
 * CHTL编译状态
 */
class CHTLState {
public:
    CHTLState();
    ~CHTLState();
    
    /**
     * 设置当前状态
     * @param state 状态名
     */
    void setState(const std::string& state);
    
    /**
     * 获取当前状态
     * @return 当前状态
     */
    std::string getCurrentState() const;
    
    /**
     * 推入状态
     * @param state 状态名
     */
    void pushState(const std::string& state);
    
    /**
     * 弹出状态
     * @return 弹出的状态
     */
    std::string popState();
    
    /**
     * 检查是否在指定状态
     * @param state 状态名
     * @return 是否在指定状态
     */
    bool isInState(const std::string& state) const;
    
    /**
     * 设置变量
     * @param name 变量名
     * @param value 变量值
     */
    void setVariable(const std::string& name, const std::string& value);
    
    /**
     * 获取变量
     * @param name 变量名
     * @return 变量值
     */
    std::string getVariable(const std::string& name) const;
    
    /**
     * 检查变量是否存在
     * @param name 变量名
     * @return 是否存在
     */
    bool hasVariable(const std::string& name) const;
    
    /**
     * 清空状态
     */
    void clear();

private:
    std::string currentState;
    std::stack<std::string> stateStack;
    std::map<std::string, std::string> variables;
};

} // namespace CHTL

#endif // CHTL_STATE_H