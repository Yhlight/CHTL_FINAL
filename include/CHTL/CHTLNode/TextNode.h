#pragma once

#include "BaseNode.h"

namespace CHTL {

/**
 * @brief 文本节点
 * 表示纯文本内容
 */
class TextNode : public BaseNode {
public:
    TextNode(const std::string& text = "");
    virtual ~TextNode();
    
    // 文本内容
    void setText(const std::string& text);
    std::string getText() const;
    
    // 是否为字面量（无修饰字符串）
    void setLiteral(bool literal);
    bool isLiteral() const;
    
    // 是否为生成器注释
    void setGeneratorComment(bool isGeneratorComment);
    bool isGeneratorComment() const;
    
    // 访问者模式
    virtual void accept(NodeVisitor& visitor) override;
    
    // 克隆节点
    virtual std::shared_ptr<BaseNode> clone() const override;
    
    // 节点验证
    virtual bool validate() const override;
    
    // 调试信息
    virtual std::string toString() const override;

private:
    std::string m_text;
    bool m_literal;
    bool m_generatorComment;
};

} // namespace CHTL