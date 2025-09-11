#include "StyleNode.h"
#include <sstream>
#include <algorithm>
#include <regex>

namespace CHTL {

StyleNode::StyleNode(bool local)
    : BaseNode(NodeType::STYLE, "style"), isLocalStyle(local) {
}

void StyleNode::setStyleProperty(const std::string& property, const std::string& value) {
    styleProperties[property] = value;
}

std::string StyleNode::getStyleProperty(const std::string& property) const {
    auto it = styleProperties.find(property);
    return (it != styleProperties.end()) ? it->second : "";
}

bool StyleNode::hasStyleProperty(const std::string& property) const {
    return styleProperties.find(property) != styleProperties.end();
}

void StyleNode::removeStyleProperty(const std::string& property) {
    styleProperties.erase(property);
}

void StyleNode::addStyleRule(std::shared_ptr<BaseNode> rule) {
    if (rule) {
        styleRules.push_back(rule);
    }
}

void StyleNode::removeStyleRule(std::shared_ptr<BaseNode> rule) {
    auto it = std::find(styleRules.begin(), styleRules.end(), rule);
    if (it != styleRules.end()) {
        styleRules.erase(it);
    }
}

void StyleNode::setInlineStyle(const std::string& style) {
    // 解析内联样式字符串
    std::istringstream iss(style);
    std::string property;
    
    while (std::getline(iss, property, ';')) {
        size_t colonPos = property.find(':');
        if (colonPos != std::string::npos) {
            std::string propName = property.substr(0, colonPos);
            std::string propValue = property.substr(colonPos + 1);
            
            // 去除空白
            propName.erase(0, propName.find_first_not_of(" \t"));
            propName.erase(propName.find_last_not_of(" \t") + 1);
            propValue.erase(0, propValue.find_first_not_of(" \t"));
            propValue.erase(propValue.find_last_not_of(" \t") + 1);
            
            if (!propName.empty() && !propValue.empty()) {
                setStyleProperty(propName, propValue);
            }
        }
    }
}

std::string StyleNode::getInlineStyle() const {
    std::ostringstream oss;
    bool first = true;
    
    for (const auto& prop : styleProperties) {
        if (!first) oss << "; ";
        oss << prop.first << ": " << prop.second;
        first = false;
    }
    
    return oss.str();
}

void StyleNode::addClassSelector(const std::string& className, const std::map<std::string, std::string>& properties) {
    auto rule = std::make_shared<BaseNode>(NodeType::ELEMENT, "rule");
    rule->setAttribute("type", "class");
    rule->setAttribute("selector", "." + className);
    
    for (const auto& prop : properties) {
        rule->setAttribute(prop.first, prop.second);
    }
    
    addStyleRule(rule);
}

void StyleNode::removeClassSelector(const std::string& className) {
    styleRules.erase(
        std::remove_if(styleRules.begin(), styleRules.end(),
            [&className](const std::shared_ptr<BaseNode>& rule) {
                return rule->getAttribute("type") == "class" && 
                       rule->getAttribute("selector") == "." + className;
            }),
        styleRules.end()
    );
}

bool StyleNode::hasClassSelector(const std::string& className) const {
    return std::any_of(styleRules.begin(), styleRules.end(),
        [&className](const std::shared_ptr<BaseNode>& rule) {
            return rule->getAttribute("type") == "class" && 
                   rule->getAttribute("selector") == "." + className;
        });
}

void StyleNode::setIdSelector(const std::string& id, const std::map<std::string, std::string>& properties) {
    auto rule = std::make_shared<BaseNode>(NodeType::ELEMENT, "rule");
    rule->setAttribute("type", "id");
    rule->setAttribute("selector", "#" + id);
    
    for (const auto& prop : properties) {
        rule->setAttribute(prop.first, prop.second);
    }
    
    addStyleRule(rule);
}

void StyleNode::removeIdSelector(const std::string& id) {
    styleRules.erase(
        std::remove_if(styleRules.begin(), styleRules.end(),
            [&id](const std::shared_ptr<BaseNode>& rule) {
                return rule->getAttribute("type") == "id" && 
                       rule->getAttribute("selector") == "#" + id;
            }),
        styleRules.end()
    );
}

bool StyleNode::hasIdSelector(const std::string& id) const {
    return std::any_of(styleRules.begin(), styleRules.end(),
        [&id](const std::shared_ptr<BaseNode>& rule) {
            return rule->getAttribute("type") == "id" && 
                   rule->getAttribute("selector") == "#" + id;
        });
}

void StyleNode::addPseudoClassSelector(const std::string& pseudoClass, const std::map<std::string, std::string>& properties) {
    auto rule = std::make_shared<BaseNode>(NodeType::ELEMENT, "rule");
    rule->setAttribute("type", "pseudo-class");
    rule->setAttribute("selector", ":" + pseudoClass);
    
    for (const auto& prop : properties) {
        rule->setAttribute(prop.first, prop.second);
    }
    
    addStyleRule(rule);
}

void StyleNode::removePseudoClassSelector(const std::string& pseudoClass) {
    styleRules.erase(
        std::remove_if(styleRules.begin(), styleRules.end(),
            [&pseudoClass](const std::shared_ptr<BaseNode>& rule) {
                return rule->getAttribute("type") == "pseudo-class" && 
                       rule->getAttribute("selector") == ":" + pseudoClass;
            }),
        styleRules.end()
    );
}

bool StyleNode::hasPseudoClassSelector(const std::string& pseudoClass) const {
    return std::any_of(styleRules.begin(), styleRules.end(),
        [&pseudoClass](const std::shared_ptr<BaseNode>& rule) {
            return rule->getAttribute("type") == "pseudo-class" && 
                   rule->getAttribute("selector") == ":" + pseudoClass;
        });
}

void StyleNode::addPseudoElementSelector(const std::string& pseudoElement, const std::map<std::string, std::string>& properties) {
    auto rule = std::make_shared<BaseNode>(NodeType::ELEMENT, "rule");
    rule->setAttribute("type", "pseudo-element");
    rule->setAttribute("selector", "::" + pseudoElement);
    
    for (const auto& prop : properties) {
        rule->setAttribute(prop.first, prop.second);
    }
    
    addStyleRule(rule);
}

void StyleNode::removePseudoElementSelector(const std::string& pseudoElement) {
    styleRules.erase(
        std::remove_if(styleRules.begin(), styleRules.end(),
            [&pseudoElement](const std::shared_ptr<BaseNode>& rule) {
                return rule->getAttribute("type") == "pseudo-element" && 
                       rule->getAttribute("selector") == "::" + pseudoElement;
            }),
        styleRules.end()
    );
}

bool StyleNode::hasPseudoElementSelector(const std::string& pseudoElement) const {
    return std::any_of(styleRules.begin(), styleRules.end(),
        [&pseudoElement](const std::shared_ptr<BaseNode>& rule) {
            return rule->getAttribute("type") == "pseudo-element" && 
                   rule->getAttribute("selector") == "::" + pseudoElement;
        });
}

void StyleNode::addContextReference(const std::string& context, const std::map<std::string, std::string>& properties) {
    auto rule = std::make_shared<BaseNode>(NodeType::ELEMENT, "rule");
    rule->setAttribute("type", "context");
    rule->setAttribute("selector", context);
    
    for (const auto& prop : properties) {
        rule->setAttribute(prop.first, prop.second);
    }
    
    addStyleRule(rule);
}

void StyleNode::removeContextReference(const std::string& context) {
    styleRules.erase(
        std::remove_if(styleRules.begin(), styleRules.end(),
            [&context](const std::shared_ptr<BaseNode>& rule) {
                return rule->getAttribute("type") == "context" && 
                       rule->getAttribute("selector") == context;
            }),
        styleRules.end()
    );
}

bool StyleNode::hasContextReference(const std::string& context) const {
    return std::any_of(styleRules.begin(), styleRules.end(),
        [&context](const std::shared_ptr<BaseNode>& rule) {
            return rule->getAttribute("type") == "context" && 
                   rule->getAttribute("selector") == context;
        });
}

std::string StyleNode::evaluatePropertyExpression(const std::string& expression) const {
    // 简化的属性表达式求值
    return parseArithmeticExpression(expression);
}

std::string StyleNode::evaluateConditionalExpression(const std::string& expression) const {
    // 简化的条件表达式求值
    return parseConditionalExpression(expression);
}

std::string StyleNode::resolvePropertyReference(const std::string& reference) const {
    // 简化的属性引用解析
    return parsePropertyReference(reference);
}

bool StyleNode::isValid() const {
    return true; // 样式节点总是有效的
}

std::string StyleNode::toHTML() const {
    if (isLocalStyle) {
        // 局部样式块不直接输出HTML
        return "";
    } else {
        // 全局样式块输出为<style>标签
        return "<style>\n" + toCSS() + "\n</style>";
    }
}

std::string StyleNode::toCSS() const {
    std::ostringstream oss;
    
    // 输出样式规则
    for (const auto& rule : styleRules) {
        std::string selector = rule->getAttribute("selector");
        if (!selector.empty()) {
            oss << selector << " {\n";
            
            for (const auto& attr : rule->getAttributes()) {
                if (attr.first != "type" && attr.first != "selector") {
                    oss << "    " << attr.first << ": " << attr.second << ";\n";
                }
            }
            
            oss << "}\n";
        }
    }
    
    // 输出内联样式属性
    if (!styleProperties.empty()) {
        oss << "/* Inline styles */\n";
        for (const auto& prop : styleProperties) {
            oss << prop.first << ": " << prop.second << ";\n";
        }
    }
    
    return oss.str();
}

std::string StyleNode::getDebugInfo() const {
    std::ostringstream oss;
    oss << BaseNode::getDebugInfo();
    oss << "IsLocalStyle: " << (isLocalStyle ? "true" : "false") << "\n";
    oss << "StyleProperties: " << styleProperties.size() << "\n";
    oss << "StyleRules: " << styleRules.size() << "\n";
    return oss.str();
}

std::shared_ptr<BaseNode> StyleNode::clone() const {
    auto cloned = std::make_shared<StyleNode>(isLocalStyle);
    cloned->styleProperties = styleProperties;
    cloned->line = line;
    cloned->column = column;
    cloned->position = position;
    
    for (const auto& rule : styleRules) {
        cloned->addStyleRule(rule->clone());
    }
    
    return cloned;
}

bool StyleNode::operator==(const StyleNode& other) const {
    return BaseNode::operator==(other) &&
           isLocalStyle == other.isLocalStyle &&
           styleProperties == other.styleProperties &&
           styleRules.size() == other.styleRules.size();
}

bool StyleNode::operator!=(const StyleNode& other) const {
    return !(*this == other);
}

std::string StyleNode::formatCSSProperty(const std::string& property, const std::string& value) const {
    return property + ": " + value + ";";
}

std::string StyleNode::formatCSSRule(const std::string& selector, const std::map<std::string, std::string>& properties) const {
    std::ostringstream oss;
    oss << selector << " {\n";
    
    for (const auto& prop : properties) {
        oss << "    " << formatCSSProperty(prop.first, prop.second) << "\n";
    }
    
    oss << "}";
    return oss.str();
}

std::string StyleNode::parseSelector(const std::string& selector) const {
    // 简化的选择器解析
    return selector;
}

std::string StyleNode::parsePropertyValue(const std::string& value) const {
    // 简化的属性值解析
    return value;
}

std::string StyleNode::parseArithmeticExpression(const std::string& expression) const {
    // 简化的算术表达式解析
    // TODO: 实现完整的算术表达式解析
    return expression;
}

std::string StyleNode::parseConditionalExpression(const std::string& expression) const {
    // 简化的条件表达式解析
    // TODO: 实现完整的条件表达式解析
    return expression;
}

std::string StyleNode::parseLogicalExpression(const std::string& expression) const {
    // 简化的逻辑表达式解析
    // TODO: 实现完整的逻辑表达式解析
    return expression;
}

std::string StyleNode::parsePropertyReference(const std::string& reference) const {
    // 简化的属性引用解析
    // TODO: 实现完整的属性引用解析
    return reference;
}

std::string StyleNode::resolveElementProperty(const std::string& selector, const std::string& property) const {
    // 简化的元素属性解析
    // TODO: 实现完整的元素属性解析
    return "";
}

} // namespace CHTL