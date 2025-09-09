#pragma once

#include "BaseNode.hpp"
#include <string>

namespace CHTL {

/**
 * @brief 文本节点
 * 
 * 表示文本内容
 */
class TextNode : public BaseNode {
public:
    TextNode(const std::string& content, size_t line = 0, size_t column = 0);
    
    // 获取文本内容
    const std::string& getContent() const { return getValue(); }
    
    // 设置文本内容
    void setContent(const std::string& content) { setValue(content); }
    
    // 检查是否为空文本
    bool isEmpty() const;
    
    // 检查是否为空白文本
    bool isWhitespace() const;
    
    // 转换为 HTML
    std::string toHTML() const override;
    
    // 转换为字符串
    std::string toString() const override;
    
private:
    // 转义 HTML 特殊字符
    std::string escapeHTML(const std::string& text) const;
};

} // namespace CHTL