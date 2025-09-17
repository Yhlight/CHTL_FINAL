#ifndef CHTL_ELEMENT_NODE_H
#define CHTL_ELEMENT_NODE_H

#include "BaseNode.h"
#include <set>

namespace CHTL {

class ElementNode : public BaseNode {
public:
    ElementNode(const std::string& tagName);
    
    // 标签相关
    const std::string& getTagName() const { return name_; }
    void setTagName(const std::string& tagName) { name_ = tagName; }
    
    // 自闭合标签
    bool isSelfClosing() const { return isSelfClosing_; }
    void setSelfClosing(bool selfClosing) { isSelfClosing_ = selfClosing; }
    
    // 内联样式
    void addInlineStyle(const std::string& property, const std::string& value);
    std::string getInlineStyle(const std::string& property) const;
    bool hasInlineStyle(const std::string& property) const;
    void removeInlineStyle(const std::string& property);
    std::string getInlineStyles() const;
    
    // 类名管理
    void addClass(const std::string& className);
    void removeClass(const std::string& className);
    bool hasClass(const std::string& className) const;
    std::string getClasses() const;
    
    // 文本内容
    void setTextContent(const std::string& text);
    std::string getTextContent() const;
    
    // 重写基类方法
    std::string toHTML() const override;
    std::string toCSS() const override;
    std::string toJS() const override;
    
    // 验证
    bool isValid() const override;
    
    // 静态方法
    static bool isSelfClosingTag(const std::string& tagName);
    static bool isBlockElement(const std::string& tagName);
    static bool isInlineElement(const std::string& tagName);
    
private:
    bool isSelfClosing_;
    std::unordered_map<std::string, std::string> inlineStyles_;
    std::set<std::string> classes_;
    
    // 静态数据
    static std::set<std::string> selfClosingTags_;
    static std::set<std::string> blockElements_;
    static std::set<std::string> inlineElements_;
    
    // 辅助方法
    std::string formatAttributes() const;
    std::string formatInlineStyles() const;
    void initializeStaticData();
};

} // namespace CHTL

#endif // CHTL_ELEMENT_NODE_H