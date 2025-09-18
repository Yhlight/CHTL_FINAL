#pragma once

#include "BaseNode.h"
#include <unordered_map>

namespace CHTL {

// 元素节点类
class ElementNode : public BaseNode {
private:
    String tagName_;
    StringMap attributes_;
    String textContent_;
    bool isSelfClosing_;
    bool isVoidElement_;
    
public:
    ElementNode(const String& tagName, const SourceLocation& loc = {})
        : BaseNode(NodeType::Element, loc), tagName_(tagName), 
          isSelfClosing_(false), isVoidElement_(false) {}
    
    // 标签名管理
    const String& getTagName() const { return tagName_; }
    void setTagName(const String& name) { tagName_ = name; }
    
    // 属性管理
    const StringMap& getAttributes() const { return attributes_; }
    void setAttribute(const String& name, const String& value);
    String getAttribute(const String& name) const;
    bool hasAttribute(const String& name) const;
    void removeAttribute(const String& name);
    void clearAttributes();
    
    // 特殊属性访问
    String getId() const { return getAttribute("id"); }
    void setId(const String& id) { setAttribute("id", id); }
    
    String getClass() const { return getAttribute("class"); }
    void setClass(const String& className) { setAttribute("class", className); }
    
    // 文本内容
    const String& getTextContent() const { return textContent_; }
    void setTextContent(const String& text) { textContent_ = text; }
    
    // 元素类型
    bool isSelfClosing() const { return isSelfClosing_; }
    void setSelfClosing(bool selfClosing) { isSelfClosing_ = selfClosing; }
    
    bool isVoidElement() const { return isVoidElement_; }
    void setVoidElement(bool voidElement) { isVoidElement_ = voidElement; }
    
    // 检查是否为特定元素类型
    bool isHTML() const { return tagName_ == "html"; }
    bool isHead() const { return tagName_ == "head"; }
    bool isBody() const { return tagName_ == "body"; }
    bool isDiv() const { return tagName_ == "div"; }
    bool isSpan() const { return tagName_ == "span"; }
    bool isP() const { return tagName_ == "p"; }
    bool isA() const { return tagName_ == "a"; }
    bool isImg() const { return tagName_ == "img"; }
    bool isInput() const { return tagName_ == "input"; }
    bool isButton() const { return tagName_ == "button"; }
    bool isForm() const { return tagName_ == "form"; }
    bool isTable() const { return tagName_ == "table"; }
    bool isTr() const { return tagName_ == "tr"; }
    bool isTd() const { return tagName_ == "td"; }
    bool isTh() const { return tagName_ == "th"; }
    bool isUl() const { return tagName_ == "ul"; }
    bool isOl() const { return tagName_ == "ol"; }
    bool isLi() const { return tagName_ == "li"; }
    bool isH1() const { return tagName_ == "h1"; }
    bool isH2() const { return tagName_ == "h2"; }
    bool isH3() const { return tagName_ == "h3"; }
    bool isH4() const { return tagName_ == "h4"; }
    bool isH5() const { return tagName_ == "h5"; }
    bool isH6() const { return tagName_ == "h6"; }
    
    // 检查是否为块级元素
    bool isBlockElement() const;
    
    // 检查是否为行内元素
    bool isInlineElement() const;
    
    // 检查是否为行内块元素
    bool isInlineBlockElement() const;
    
    // 检查是否为自闭合元素
    bool isSelfClosingElement() const;
    
    // 检查是否为void元素
    bool isVoidElementType() const;
    
    // 查找子元素
    ElementNodePtr findElement(const String& tagName) const;
    std::vector<ElementNodePtr> findElements(const String& tagName) const;
    ElementNodePtr findElementById(const String& id) const;
    std::vector<ElementNodePtr> findElementsByClass(const String& className) const;
    
    // 查找样式节点
    StyleNodePtr findStyleNode() const;
    
    // 查找脚本节点
    ScriptNodePtr findScriptNode() const;
    
    // 实现基类方法
    String toString() const override;
    String toHTML() const override;
    bool isValid() const override;
    void accept(NodeVisitor& visitor) override;
    
    // 创建元素节点的工厂方法
    static ElementNodePtr create(const String& tagName, const SourceLocation& loc = {});
    static ElementNodePtr createHTML(const SourceLocation& loc = {});
    static ElementNodePtr createHead(const SourceLocation& loc = {});
    static ElementNodePtr createBody(const SourceLocation& loc = {});
    static ElementNodePtr createDiv(const SourceLocation& loc = {});
    static ElementNodePtr createSpan(const SourceLocation& loc = {});
    static ElementNodePtr createP(const SourceLocation& loc = {});
    static ElementNodePtr createA(const String& href, const SourceLocation& loc = {});
    static ElementNodePtr createImg(const String& src, const String& alt = "", const SourceLocation& loc = {});
    static ElementNodePtr createInput(const String& type, const SourceLocation& loc = {});
    static ElementNodePtr createButton(const String& text, const SourceLocation& loc = {});
    static ElementNodePtr createForm(const String& action, const SourceLocation& loc = {});
    static ElementNodePtr createTable(const SourceLocation& loc = {});
    static ElementNodePtr createTr(const SourceLocation& loc = {});
    static ElementNodePtr createTd(const SourceLocation& loc = {});
    static ElementNodePtr createTh(const SourceLocation& loc = {});
    static ElementNodePtr createUl(const SourceLocation& loc = {});
    static ElementNodePtr createOl(const SourceLocation& loc = {});
    static ElementNodePtr createLi(const SourceLocation& loc = {});
    static ElementNodePtr createH1(const SourceLocation& loc = {});
    static ElementNodePtr createH2(const SourceLocation& loc = {});
    static ElementNodePtr createH3(const SourceLocation& loc = {});
    static ElementNodePtr createH4(const SourceLocation& loc = {});
    static ElementNodePtr createH5(const SourceLocation& loc = {});
    static ElementNodePtr createH6(const SourceLocation& loc = {});
};

} // namespace CHTL