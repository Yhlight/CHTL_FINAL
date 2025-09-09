#pragma once

#include "CHTLJSBaseNode.hpp"
#include <string>

namespace CHTL {

/**
 * @brief 增强选择器节点
 * 
 * 根据 CHTL.md 文档第 1266-1297 行实现
 * 支持 {{CSS选择器}} 语法创建 DOM 对象
 */
class EnhancedSelectorNode : public CHTLJSBaseNode {
public:
    EnhancedSelectorNode(const std::string& selector, size_t line = 0, size_t column = 0);
    
    // 获取选择器
    const std::string& getSelector() const { return selector_; }
    
    // 设置选择器
    void setSelector(const std::string& selector) { selector_ = selector; }
    
    // 检查是否为精确访问
    bool isPreciseAccess() const;
    
    // 获取精确访问索引
    int getPreciseIndex() const;
    
    // 转换为 JavaScript 代码
    std::string toJavaScript() const override;
    
    // 转换为字符串
    std::string toString() const override;
    
private:
    std::string selector_;
    
    // 解析选择器类型
    std::string parseSelectorType() const;
    
    // 生成 DOM 查询代码
    std::string generateDOMQuery() const;
};

} // namespace CHTL