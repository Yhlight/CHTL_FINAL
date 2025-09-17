#pragma once

#include "BaseNode.h"

namespace CHTL {

// 文本节点类
class TextNode : public BaseNode {
private:
    String content_;
    bool isRawText_;  // 是否为原始文本（不需要转义）
    
public:
    TextNode(const String& content, const SourceLocation& loc = {})
        : BaseNode(NodeType::Text, loc), content_(content), isRawText_(false) {}
    
    // 内容管理
    const String& getContent() const { return content_; }
    void setContent(const String& content) { content_ = content; }
    
    // 原始文本标记
    bool isRawText() const { return isRawText_; }
    void setRawText(bool raw) { isRawText_ = raw; }
    
    // 文本操作
    void append(const String& text) { content_ += text; }
    void prepend(const String& text) { content_ = text + content_; }
    void trim() { 
        // 简单的trim实现
        size_t start = content_.find_first_not_of(" \t\n\r");
        if (start != String::npos) {
            size_t end = content_.find_last_not_of(" \t\n\r");
            content_ = content_.substr(start, end - start + 1);
        } else {
            content_.clear();
        }
    }
    
    // 检查是否为空
    bool isEmpty() const { return content_.empty(); }
    
    // 检查是否只包含空白字符
    bool isWhitespace() const;
    
    // 转义HTML特殊字符
    String escapeHTML() const;
    
    // 实现基类方法
    String toString() const override;
    String toHTML() const override;
    bool isValid() const override;
    void accept(NodeVisitor& visitor) override;
    
    // 创建文本节点的工厂方法
    static TextNodePtr create(const String& content, const SourceLocation& loc = {});
    static TextNodePtr createRaw(const String& content, const SourceLocation& loc = {});
    static TextNodePtr createEmpty(const SourceLocation& loc = {});
    static TextNodePtr createWhitespace(const SourceLocation& loc = {});
};

} // namespace CHTL