#pragma once

#include "BaseNode.hpp"
#include <string>

namespace CHTL {

/**
 * @brief 元素节点
 * 
 * 表示 HTML 元素，如 div, span, p 等
 */
class ElementNode : public BaseNode {
public:
    ElementNode(const std::string& tagName, size_t line = 0, size_t column = 0);
    
    // 获取标签名
    const std::string& getTagName() const { return tagName_; }
    
    // 设置标签名
    void setTagName(const std::string& tagName) { tagName_ = tagName; }
    
    // 检查是否为自闭合标签
    bool isSelfClosing() const;
    
    // 设置自闭合状态
    void setSelfClosing(bool selfClosing) { selfClosing_ = selfClosing; }
    
    // 检查是否为块级元素
    bool isBlockElement() const;
    
    // 检查是否为行内元素
    bool isInlineElement() const;
    
    // 检查是否为行内块元素
    bool isInlineBlockElement() const;
    
    // 转换为 HTML
    std::string toHTML() const override;
    
    // 转换为字符串
    std::string toString() const override;
    
private:
    std::string tagName_;
    bool selfClosing_;
    
    // 检查标签是否为指定类型
    bool isTagOfType(const std::vector<std::string>& tagList) const;
};

} // namespace CHTL