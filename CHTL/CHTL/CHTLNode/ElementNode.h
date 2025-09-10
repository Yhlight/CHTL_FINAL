#ifndef CHTL_ELEMENT_NODE_H
#define CHTL_ELEMENT_NODE_H

#include "BaseNode.h"
#include <set>

namespace CHTL {

class ElementNode : public BaseNode {
private:
    bool isSelfClosing;  // 是否为自闭合标签
    bool isVoidElement;  // 是否为void元素
    std::set<std::string> voidElements;  // void元素集合
    
    void initializeVoidElements();

public:
    ElementNode(const std::string& tagName = "", bool selfClosing = false);
    virtual ~ElementNode() = default;
    
    // 标签名操作
    const std::string& getTagName() const { return name; }
    void setTagName(const std::string& tagName) { name = tagName; }
    
    // 自闭合标签
    bool isSelfClosingTag() const { return isSelfClosing; }
    void setSelfClosing(bool selfClosing) { isSelfClosing = selfClosing; }
    
    // void元素
    bool isVoidElementTag() const { return isVoidElement; }
    bool isVoidElementTag(const std::string& tagName) const;
    
    // 属性操作（重写基类方法以支持CHTL特殊语法）
    void setAttribute(const std::string& key, const std::string& value);
    std::string getAttribute(const std::string& key) const;
    
    // 特殊属性处理
    void setClass(const std::string& className);
    void addClass(const std::string& className);
    void removeClass(const std::string& className);
    bool hasClass(const std::string& className) const;
    std::vector<std::string> getClasses() const;
    
    void setId(const std::string& id);
    std::string getId() const;
    
    void setStyle(const std::string& style);
    std::string getStyle() const;
    
    // 文本内容
    void setTextContent(const std::string& text);
    std::string getTextContent() const;
    
    // 子元素操作
    void addElement(std::shared_ptr<ElementNode> element);
    std::vector<std::shared_ptr<ElementNode>> getElements() const;
    std::vector<std::shared_ptr<ElementNode>> getElementsByTagName(const std::string& tagName) const;
    std::vector<std::shared_ptr<ElementNode>> getElementsByClassName(const std::string& className) const;
    std::shared_ptr<ElementNode> getElementById(const std::string& id) const;
    
    // 查询选择器
    std::vector<std::shared_ptr<ElementNode>> querySelectorAll(const std::string& selector) const;
    std::shared_ptr<ElementNode> querySelector(const std::string& selector) const;
    
    // 验证
    bool isValid() const override;
    
    // 转换为HTML
    std::string toHTML() const override;
    std::string toHTML(int indentLevel = 0) const;
    
    // 调试信息
    std::string getDebugInfo() const override;
    
    // 克隆
    std::shared_ptr<BaseNode> clone() const override;
    
    // 比较操作
    bool operator==(const ElementNode& other) const;
    bool operator!=(const ElementNode& other) const;

private:
    // 辅助方法
    std::string attributesToHTML() const;
    std::string indent(int level) const;
    bool isInlineElement(const std::string& tagName) const;
public:
    bool isBlockElement(const std::string& tagName) const;
    bool isInlineBlockElement(const std::string& tagName) const;
};

} // namespace CHTL

#endif // CHTL_ELEMENT_NODE_H