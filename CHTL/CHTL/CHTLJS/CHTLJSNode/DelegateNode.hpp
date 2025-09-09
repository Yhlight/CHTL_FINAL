#pragma once

#include "CHTLJSBaseNode.hpp"
#include <string>
#include <map>
#include <vector>

namespace CHTL {

/**
 * @brief 事件委托节点
 * 
 * 表示 delegate {} 语法
 */
class DelegateNode : public CHTLJSBaseNode {
public:
    DelegateNode(size_t line = 0, size_t column = 0);
    
    // 设置目标选择器
    void setTarget(const std::string& target);
    
    // 添加目标选择器
    void addTarget(const std::string& target);
    
    // 获取所有目标选择器
    const std::vector<std::string>& getTargets() const { return targets_; }
    
    // 添加事件委托
    void addEventDelegate(const std::string& eventType, const std::string& handler);
    
    // 获取所有事件委托
    const std::map<std::string, std::string>& getEventDelegates() const { return eventDelegates_; }
    
    // 转换为 JavaScript 代码
    std::string toJavaScript() const override;
    
    // 转换为字符串
    std::string toString() const override;
    
private:
    std::vector<std::string> targets_;
    std::map<std::string, std::string> eventDelegates_;
};

} // namespace CHTL