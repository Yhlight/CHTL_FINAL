#ifndef CHTL_TEXT_NODE_H
#define CHTL_TEXT_NODE_H

#include "BaseNode.h"

namespace CHTL {

class TextNode : public BaseNode {
private:
    bool isLiteral;  // 是否为无修饰字面量
    bool needsEscape;  // 是否需要HTML转义

public:
    TextNode(const std::string& text = "", bool isLiteral = false);
    virtual ~TextNode() = default;
    
    // 文本内容
    const std::string& getText() const { return value; }
    void setText(const std::string& text) { value = text; }
    
    // 字面量处理
    bool isLiteralText() const { return isLiteral; }
    void setLiteral(bool literal) { isLiteral = literal; }
    
    // HTML转义
    bool needsHTMLEscape() const { return needsEscape; }
    void setNeedsEscape(bool escape) { needsEscape = escape; }
    
    // 文本处理
    std::string getTrimmedText() const;
    std::string getEscapedText() const;
    bool isEmpty() const;
    bool isWhitespace() const;
    
    // 验证
    bool isValid() const override;
    
    // 转换为HTML
    std::string toHTML() const override;
    
    // 调试信息
    std::string getDebugInfo() const override;
    
    // 克隆
    std::shared_ptr<BaseNode> clone() const override;
    
    // 比较操作
    bool operator==(const TextNode& other) const;
    bool operator!=(const TextNode& other) const;

private:
    // 辅助方法
    std::string escapeHTML(const std::string& text) const;
    std::string unescapeHTML(const std::string& text) const;
    bool isWhitespaceChar(char c) const;
};

} // namespace CHTL

#endif // CHTL_TEXT_NODE_H