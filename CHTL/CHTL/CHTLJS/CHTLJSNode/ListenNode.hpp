#pragma once

#include "CHTLJSBaseNode.hpp"
#include <string>
#include <map>

namespace CHTL {

/**
 * @brief 增强监听器节点
 * 
 * 表示 listen {} 语法
 */
class ListenNode : public CHTLJSBaseNode {
public:
    ListenNode(size_t line = 0, size_t column = 0);
    
    // 添加事件监听器
    void addEventListener(const std::string& eventType, const std::string& handler);
    
    // 获取所有事件监听器
    const std::map<std::string, std::string>& getEventListeners() const { return eventListeners_; }
    
    // 转换为 JavaScript 代码
    std::string toJavaScript() const override;
    
    // 转换为字符串
    std::string toString() const override;
    
private:
    std::map<std::string, std::string> eventListeners_;
};

} // namespace CHTL