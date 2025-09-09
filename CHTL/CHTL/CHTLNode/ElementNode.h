#pragma once

#include "BaseNode.h"

namespace CHTL {

class ElementNode : public BaseNode {
private:
    bool isSelfClosing;
    bool isVoidElement;
    
    // HTML5 void elements
    static const std::vector<std::string> VOID_ELEMENTS;
    
public:
    ElementNode(const std::string& tagName = "");
    virtual ~ElementNode() = default;
    
    // 元素类型判断
    bool isSelfClosingElement() const { return isSelfClosing; }
    bool isVoidElementType() const { return isVoidElement; }
    void setSelfClosing(bool selfClosing) { isSelfClosing = selfClosing; }
    
    // 标签名相关
    std::string getTagName() const { return name; }
    void setTagName(const std::string& tagName);
    
    // 属性处理
    void addClass(const std::string& className);
    void removeClass(const std::string& className);
    bool hasClass(const std::string& className) const;
    std::vector<std::string> getClasses() const;
    
    void setId(const std::string& id);
    std::string getId() const;
    bool hasId() const;
    
    // 内容处理
    void setTextContent(const std::string& text);
    std::string getTextContent() const;
    
    // HTML生成
    virtual std::string toHTML() const override;
    std::string toHTML(bool prettyPrint = false, int indentLevel = 0) const;
    
    // 验证
    virtual bool isValid() const override;
    bool isValidHTML5() const;
    
    // 克隆
    virtual std::shared_ptr<BaseNode> clone() const override;
    
    // 调试
    virtual std::string debugString() const override;
    
    // 静态方法
    static bool isVoidElement(const std::string& tagName);
    static bool isSelfClosingElement(const std::string& tagName);
    static std::vector<std::string> getVoidElements();
};

} // namespace CHTL