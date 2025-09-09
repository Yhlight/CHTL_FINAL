#pragma once

#include "BaseNode.h"

namespace CHTL {

class TextNode : public BaseNode {
private:
    std::string content;
    bool isRaw; // 是否为原始文本（不需要HTML转义）
    
public:
    TextNode(const std::string& content = "", bool isRaw = false);
    virtual ~TextNode() = default;
    
    // 内容管理
    const std::string& getContent() const { return content; }
    void setContent(const std::string& content) { this->content = content; }
    
    // 原始文本处理
    bool isRawText() const { return isRaw; }
    void setRaw(bool raw) { isRaw = raw; }
    
    // 文本处理
    void append(const std::string& text);
    void prepend(const std::string& text);
    void trim();
    bool isEmpty() const;
    size_t length() const;
    
    // HTML转义
    std::string escapeHTML() const;
    std::string unescapeHTML() const;
    
    // HTML生成
    virtual std::string toHTML() const override;
    std::string toHTML(bool escape = true) const;
    
    // 验证
    virtual bool isValid() const override;
    
    // 克隆
    virtual std::shared_ptr<BaseNode> clone() const override;
    
    // 调试
    virtual std::string debugString() const override;
    
    // 静态方法
    static std::string escapeHTMLString(const std::string& str);
    static std::string unescapeHTMLString(const std::string& str);
};

} // namespace CHTL