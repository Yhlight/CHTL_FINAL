#pragma once

#include "BaseNode.h"
#include <unordered_map>
#include <vector>

namespace CHTL {

// 样式属性
struct StyleProperty {
    String name;
    String value;
    SourceLocation location;
    bool important;
    
    StyleProperty(const String& n, const String& v, const SourceLocation& loc = {}, bool imp = false)
        : name(n), value(v), location(loc), important(imp) {}
};

// 样式规则
struct StyleRule {
    String selector;
    std::vector<StyleProperty> properties;
    SourceLocation location;
    
    StyleRule(const String& sel, const SourceLocation& loc = {})
        : selector(sel), location(loc) {}
};

// 样式节点类
class StyleNode : public BaseNode {
private:
    String content_;
    std::vector<StyleRule> rules_;
    std::unordered_map<String, String> inlineProperties_;
    bool isGlobal_;  // 是否为全局样式
    bool isLocal_;   // 是否为局部样式
    
public:
    StyleNode(const SourceLocation& loc = {})
        : BaseNode(NodeType::Style, loc), isGlobal_(false), isLocal_(false) {}
    
    // 内容管理
    const String& getContent() const { return content_; }
    void setContent(const String& content) { content_ = content; }
    
    // 样式类型
    bool isGlobal() const { return isGlobal_; }
    void setGlobal(bool global) { isGlobal_ = global; }
    
    bool isLocal() const { return isLocal_; }
    void setLocal(bool local) { isLocal_ = local; }
    
    // 内联样式管理
    const std::unordered_map<String, String>& getInlineProperties() const { return inlineProperties_; }
    void setInlineProperty(const String& name, const String& value);
    String getInlineProperty(const String& name) const;
    bool hasInlineProperty(const String& name) const;
    void removeInlineProperty(const String& name);
    void clearInlineProperties();
    
    // 样式规则管理
    const std::vector<StyleRule>& getRules() const { return rules_; }
    void addRule(const StyleRule& rule);
    void addRule(const String& selector, const SourceLocation& loc = {});
    void addProperty(const String& selector, const StyleProperty& property);
    void addProperty(const String& selector, const String& name, const String& value, const SourceLocation& loc = {});
    void clearRules();
    
    // 查找规则
    StyleRule* findRule(const String& selector);
    const StyleRule* findRule(const String& selector) const;
    std::vector<StyleRule*> findRulesBySelector(const String& pattern);
    std::vector<const StyleRule*> findRulesBySelector(const String& pattern) const;
    
    // 属性操作
    void setProperty(const String& selector, const String& name, const String& value, bool important = false);
    String getProperty(const String& selector, const String& name) const;
    bool hasProperty(const String& selector, const String& name) const;
    void removeProperty(const String& selector, const String& name);
    
    // 选择器操作
    void addClassSelector(const String& className);
    void addIdSelector(const String& id);
    void addPseudoClassSelector(const String& pseudoClass);
    void addPseudoElementSelector(const String& pseudoElement);
    void addAttributeSelector(const String& attribute, const String& value = "");
    void addDescendantSelector(const String& ancestor, const String& descendant);
    void addChildSelector(const String& parent, const String& child);
    void addAdjacentSelector(const String& sibling, const String& adjacent);
    void addGeneralSiblingSelector(const String& sibling, const String& general);
    
    // 特殊选择器
    void addContextualSelector(const String& selector);  // & 选择器
    void addIndexSelector(const String& baseSelector, size_t index);
    
    // 样式生成
    String generateCSS() const;
    String generateInlineStyle() const;
    
    // 验证
    bool validateSelector(const String& selector) const;
    bool validateProperty(const String& name, const String& value) const;
    
    // 实现基类方法
    String toString() const override;
    String toHTML() const override;
    bool isValid() const override;
    void accept(NodeVisitor& visitor) override;
    
    // 创建样式节点的工厂方法
    static StyleNodePtr create(const SourceLocation& loc = {});
    static StyleNodePtr createGlobal(const SourceLocation& loc = {});
    static StyleNodePtr createLocal(const SourceLocation& loc = {});
    static StyleNodePtr createWithContent(const String& content, const SourceLocation& loc = {});
};

} // namespace CHTL