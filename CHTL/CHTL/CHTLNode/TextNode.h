#ifndef TEXT_NODE_H
#define TEXT_NODE_H

#include "BaseNode.h"

namespace CHTL {

/**
 * 文本节点
 */
class TextNode : public BaseNode {
public:
    TextNode(const std::string& text = "");
    virtual ~TextNode() = default;
    
    // 获取/设置文本内容
    const std::string& getText() const { return m_value; }
    void setText(const std::string& text) { m_value = text; }
    
    // 检查是否为空文本
    bool isEmpty() const { return m_value.empty(); }
    
    // 检查是否为空白文本
    bool isWhitespace() const;
    
    // 克隆节点
    std::shared_ptr<BaseNode> clone() const override;
    
    // 转换为字符串
    std::string toString() const override;
    
    // 接受访问者
    void accept(NodeVisitor& visitor) override;
};

} // namespace CHTL

#endif // TEXT_NODE_H