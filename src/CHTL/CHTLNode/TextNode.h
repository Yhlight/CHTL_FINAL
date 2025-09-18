#ifndef CHTL_TEXT_NODE_H
#define CHTL_TEXT_NODE_H

#include "BaseNode.h"

namespace CHTL {

class TextNode : public BaseNode {
public:
    TextNode(const std::string& text = "");
    
    // 文本内容
    void setText(const std::string& text) { content_ = text; }
    const std::string& getText() const { return content_; }
    
    // 文本处理
    void appendText(const std::string& text);
    void prependText(const std::string& text);
    void trimText();
    
    // 重写基类方法
    std::string toHTML() const override;
    std::string toString() const override;
    
    // 验证
    bool isValid() const override;
    
    // 静态工厂方法
    static std::shared_ptr<TextNode> create(const std::string& text);
    
private:
    // 辅助方法
    std::string processText() const;
    bool isWhitespaceOnly() const;
};

} // namespace CHTL

#endif // CHTL_TEXT_NODE_H