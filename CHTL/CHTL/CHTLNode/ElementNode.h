#ifndef ELEMENT_NODE_H
#define ELEMENT_NODE_H

#include "BaseNode.h"

namespace CHTL {

/**
 * 元素节点
 */
class ElementNode : public BaseNode {
public:
    ElementNode(const std::string& tagName);
    virtual ~ElementNode() = default;
    
    // 获取/设置标签名
    const std::string& getTagName() const { return m_tagName; }
    void setTagName(const std::string& tagName) { m_tagName = tagName; }
    
    // 检查是否为自闭合标签
    bool isSelfClosing() const { return m_selfClosing; }
    void setSelfClosing(bool selfClosing) { m_selfClosing = selfClosing; }
    
    // 检查是否为HTML5元素
    bool isHTML5Element() const;
    
    // 获取属性
    const std::map<std::string, std::string>& getAttributes() const { return m_attributes; }
    
    // 克隆节点
    std::shared_ptr<BaseNode> clone() const override;
    
    // 转换为字符串
    std::string toString() const override;
    
    // 接受访问者
    void accept(NodeVisitor& visitor) override;

private:
    std::string m_tagName;
    bool m_selfClosing;
    
    // HTML5自闭合标签列表
    static const std::vector<std::string> s_selfClosingTags;
    static const std::vector<std::string> s_html5Tags;
};

} // namespace CHTL

#endif // ELEMENT_NODE_H