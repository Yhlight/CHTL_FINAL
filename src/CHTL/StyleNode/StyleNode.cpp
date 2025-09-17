#include "CHTL/StyleNode.h"
#include <sstream>
#include <regex>

namespace CHTL {

// 内联样式管理
void StyleNode::setInlineProperty(const String& name, const String& value) {
    inlineProperties_[name] = value;
}

String StyleNode::getInlineProperty(const String& name) const {
    auto it = inlineProperties_.find(name);
    return it != inlineProperties_.end() ? it->second : "";
}

bool StyleNode::hasInlineProperty(const String& name) const {
    return inlineProperties_.find(name) != inlineProperties_.end();
}

void StyleNode::removeInlineProperty(const String& name) {
    inlineProperties_.erase(name);
}

void StyleNode::clearInlineProperties() {
    inlineProperties_.clear();
}

// 样式规则管理
void StyleNode::addRule(const StyleRule& rule) {
    rules_.push_back(rule);
}

void StyleNode::addRule(const String& selector, const SourceLocation& loc) {
    rules_.emplace_back(selector, loc);
}

void StyleNode::addProperty(const String& selector, const StyleProperty& property) {
    // 查找现有规则
    for (auto& rule : rules_) {
        if (rule.selector == selector) {
            rule.properties.push_back(property);
            return;
        }
    }
    
    // 创建新规则
    StyleRule newRule(selector);
    newRule.properties.push_back(property);
    rules_.push_back(newRule);
}

void StyleNode::addProperty(const String& selector, const String& name, const String& value, const SourceLocation& loc) {
    addProperty(selector, StyleProperty(name, value, loc));
}

void StyleNode::clearRules() {
    rules_.clear();
}

// 查找规则
StyleRule* StyleNode::findRule(const String& selector) {
    for (auto& rule : rules_) {
        if (rule.selector == selector) {
            return &rule;
        }
    }
    return nullptr;
}

const StyleRule* StyleNode::findRule(const String& selector) const {
    for (const auto& rule : rules_) {
        if (rule.selector == selector) {
            return &rule;
        }
    }
    return nullptr;
}

std::vector<StyleRule*> StyleNode::findRulesBySelector(const String& pattern) {
    std::vector<StyleRule*> result;
    std::regex regexPattern(pattern);
    
    for (auto& rule : rules_) {
        if (std::regex_search(rule.selector, regexPattern)) {
            result.push_back(&rule);
        }
    }
    return result;
}

std::vector<const StyleRule*> StyleNode::findRulesBySelector(const String& pattern) const {
    std::vector<const StyleRule*> result;
    std::regex regexPattern(pattern);
    
    for (const auto& rule : rules_) {
        if (std::regex_search(rule.selector, regexPattern)) {
            result.push_back(&rule);
        }
    }
    return result;
}

// 属性操作
void StyleNode::setProperty(const String& selector, const String& name, const String& value, bool important) {
    StyleRule* rule = findRule(selector);
    if (!rule) {
        addRule(selector);
        rule = &rules_.back();
    }
    
    // 查找现有属性
    for (auto& prop : rule->properties) {
        if (prop.name == name) {
            prop.value = value;
            prop.important = important;
            return;
        }
    }
    
    // 添加新属性
    rule->properties.emplace_back(name, value, SourceLocation{}, important);
}

String StyleNode::getProperty(const String& selector, const String& name) const {
    const StyleRule* rule = findRule(selector);
    if (!rule) {
        return "";
    }
    
    for (const auto& prop : rule->properties) {
        if (prop.name == name) {
            return prop.value;
        }
    }
    return "";
}

bool StyleNode::hasProperty(const String& selector, const String& name) const {
    const StyleRule* rule = findRule(selector);
    if (!rule) {
        return false;
    }
    
    for (const auto& prop : rule->properties) {
        if (prop.name == name) {
            return true;
        }
    }
    return false;
}

void StyleNode::removeProperty(const String& selector, const String& name) {
    StyleRule* rule = findRule(selector);
    if (!rule) {
        return;
    }
    
    auto it = std::find_if(rule->properties.begin(), rule->properties.end(),
        [&name](const StyleProperty& prop) {
            return prop.name == name;
        });
    
    if (it != rule->properties.end()) {
        rule->properties.erase(it);
    }
}

// 选择器操作
void StyleNode::addClassSelector(const String& className) {
    addRule("." + className);
}

void StyleNode::addIdSelector(const String& id) {
    addRule("#" + id);
}

void StyleNode::addPseudoClassSelector(const String& pseudoClass) {
    addRule(":" + pseudoClass);
}

void StyleNode::addPseudoElementSelector(const String& pseudoElement) {
    addRule("::" + pseudoElement);
}

void StyleNode::addAttributeSelector(const String& attribute, const String& value) {
    String selector = "[" + attribute;
    if (!value.empty()) {
        selector += "=\"" + value + "\"";
    }
    selector += "]";
    addRule(selector);
}

void StyleNode::addDescendantSelector(const String& ancestor, const String& descendant) {
    addRule(ancestor + " " + descendant);
}

void StyleNode::addChildSelector(const String& parent, const String& child) {
    addRule(parent + " > " + child);
}

void StyleNode::addAdjacentSelector(const String& sibling, const String& adjacent) {
    addRule(sibling + " + " + adjacent);
}

void StyleNode::addGeneralSiblingSelector(const String& sibling, const String& general) {
    addRule(sibling + " ~ " + general);
}

void StyleNode::addContextualSelector(const String& selector) {
    addRule(selector);
}

void StyleNode::addIndexSelector(const String& baseSelector, size_t index) {
    addRule(baseSelector + "[" + std::to_string(index) + "]");
}

// 样式生成
String StyleNode::generateCSS() const {
    std::ostringstream oss;
    
    for (const auto& rule : rules_) {
        oss << rule.selector << " {\n";
        for (const auto& prop : rule.properties) {
            oss << "  " << prop.name << ": " << prop.value;
            if (prop.important) {
                oss << " !important";
            }
            oss << ";\n";
        }
        oss << "}\n\n";
    }
    
    return oss.str();
}

String StyleNode::generateInlineStyle() const {
    std::ostringstream oss;
    
    bool first = true;
    for (const auto& prop : inlineProperties_) {
        if (!first) {
            oss << "; ";
        }
        oss << prop.first << ": " << prop.second;
        first = false;
    }
    
    return oss.str();
}

// 验证
bool StyleNode::validateSelector(const String& selector) const {
    // 简单的选择器验证
    if (selector.empty()) {
        return false;
    }
    
    // 检查基本语法
    std::regex validSelector(R"([.#]?[a-zA-Z_][a-zA-Z0-9_-]*(?:\s*[.#]?[a-zA-Z_][a-zA-Z0-9_-]*)*)");
    return std::regex_match(selector, validSelector);
}

bool StyleNode::validateProperty(const String& name, const String& value) const {
    // 简单的属性验证
    if (name.empty() || value.empty()) {
        return false;
    }
    
    // 检查属性名格式
    std::regex validPropertyName(R"([a-zA-Z-]+)");
    if (!std::regex_match(name, validPropertyName)) {
        return false;
    }
    
    return true;
}

// 实现基类方法
String StyleNode::toString() const {
    if (isGlobal_) {
        return "<style>" + generateCSS() + "</style>";
    } else if (isLocal_) {
        return "style { " + generateInlineStyle() + " }";
    } else {
        return generateCSS();
    }
}

String StyleNode::toHTML() const {
    if (isGlobal_) {
        return "<style>\n" + generateCSS() + "</style>";
    } else {
        return generateInlineStyle();
    }
}

bool StyleNode::isValid() const {
    // 验证所有规则
    for (const auto& rule : rules_) {
        if (!validateSelector(rule.selector)) {
            return false;
        }
        
        for (const auto& prop : rule.properties) {
            if (!validateProperty(prop.name, prop.value)) {
                return false;
            }
        }
    }
    
    // 验证内联属性
    for (const auto& prop : inlineProperties_) {
        if (!validateProperty(prop.first, prop.second)) {
            return false;
        }
    }
    
    return true;
}

void StyleNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

// 工厂方法
StyleNodePtr StyleNode::create(const SourceLocation& loc) {
    return std::make_shared<StyleNode>(loc);
}

StyleNodePtr StyleNode::createGlobal(const SourceLocation& loc) {
    auto node = create(loc);
    node->setGlobal(true);
    return node;
}

StyleNodePtr StyleNode::createLocal(const SourceLocation& loc) {
    auto node = create(loc);
    node->setLocal(true);
    return node;
}

StyleNodePtr StyleNode::createWithContent(const String& content, const SourceLocation& loc) {
    auto node = create(loc);
    node->setContent(content);
    return node;
}

} // namespace CHTL