#pragma once

#include "BaseNode.h"

namespace CHTL {

/**
 * @brief HTML元素节点
 * 表示HTML元素，如div、span、p等
 */
class ElementNode : public BaseNode {
public:
    ElementNode(const std::string& tagName);
    virtual ~ElementNode();
    
    // 标签名
    void setTagName(const std::string& tagName);
    std::string getTagName() const;
    
    // 是否为自闭合标签
    void setSelfClosing(bool selfClosing);
    bool isSelfClosing() const;
    
    // 是否为块级元素
    bool isBlockElement() const;
    
    // 是否为行内元素
    bool isInlineElement() const;
    
    // 文本内容
    void setTextContent(const std::string& text);
    std::string getTextContent() const;
    
    // 样式属性
    void addStyleProperty(const std::string& property, const std::string& value);
    std::string getStyleProperty(const std::string& property) const;
    bool hasStyleProperty(const std::string& property) const;
    void removeStyleProperty(const std::string& property);
    const std::unordered_map<std::string, std::string>& getStyleProperties() const;
    
    // 类名管理
    void addClass(const std::string& className);
    void removeClass(const std::string& className);
    bool hasClass(const std::string& className) const;
    std::vector<std::string> getClasses() const;
    void setClasses(const std::vector<std::string>& classes);
    
    // ID管理
    void setID(const std::string& id);
    std::string getID() const;
    bool hasID() const;
    
    // 访问者模式
    virtual void accept(NodeVisitor& visitor) override;
    
    // 克隆节点
    virtual std::shared_ptr<BaseNode> clone() const override;
    
    // 节点验证
    virtual bool validate() const override;
    
    // 调试信息
    virtual std::string toString() const override;

private:
    std::string m_tagName;
    bool m_selfClosing;
    std::string m_textContent;
    std::unordered_map<std::string, std::string> m_styleProperties;
    std::vector<std::string> m_classes;
    std::string m_id;
    
    // 辅助方法
    bool isBlockElementTag(const std::string& tagName) const;
    bool isInlineElementTag(const std::string& tagName) const;
    bool isSelfClosingTag(const std::string& tagName) const;
};

} // namespace CHTL