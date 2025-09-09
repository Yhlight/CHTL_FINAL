#include "StyleNode.h"
#include <algorithm>
#include <sstream>

namespace CHTL {

// StyleProperty implementation
StyleProperty::StyleProperty(const std::string& name, const std::string& value)
    : m_name(name), m_value(value), m_important(false) {}

std::string StyleProperty::toCSS() const {
    std::stringstream ss;
    ss << m_name << ": " << m_value;
    if (m_important) {
        ss << " !important";
    }
    ss << ";";
    return ss.str();
}

// StyleSelector implementation
StyleSelector::StyleSelector(Type type, const std::string& value)
    : m_type(type), m_value(value) {}

std::string StyleSelector::toCSS() const {
    switch (m_type) {
        case Type::TAG:
            return m_value;
        case Type::CLASS:
            return "." + m_value;
        case Type::ID:
            return "#" + m_value;
        case Type::PSEUDO:
            return ":" + m_value;
        case Type::PSEUDO_ELEMENT:
            return "::" + m_value;
        case Type::DESCENDANT:
            return m_value;
        case Type::CHILD:
            return "> " + m_value;
        case Type::ADJACENT:
            return "+ " + m_value;
        case Type::SIBLING:
            return "~ " + m_value;
        case Type::ATTRIBUTE:
            return "[" + m_value + "]";
        case Type::UNIVERSAL:
            return "*";
        default:
            return m_value;
    }
}

std::string StyleSelector::typeToString(Type type) {
    switch (type) {
        case Type::TAG: return "TAG";
        case Type::CLASS: return "CLASS";
        case Type::ID: return "ID";
        case Type::PSEUDO: return "PSEUDO";
        case Type::PSEUDO_ELEMENT: return "PSEUDO_ELEMENT";
        case Type::DESCENDANT: return "DESCENDANT";
        case Type::CHILD: return "CHILD";
        case Type::ADJACENT: return "ADJACENT";
        case Type::SIBLING: return "SIBLING";
        case Type::ATTRIBUTE: return "ATTRIBUTE";
        case Type::UNIVERSAL: return "UNIVERSAL";
        default: return "UNKNOWN";
    }
}

// StyleRule implementation
StyleRule::StyleRule() {}

void StyleRule::addSelector(std::unique_ptr<StyleSelector> selector) {
    m_selectors.push_back(std::move(selector));
}

void StyleRule::addProperty(std::unique_ptr<StyleProperty> property) {
    m_properties.push_back(std::move(property));
}

std::string StyleRule::toCSS() const {
    if (isEmpty()) {
        return "";
    }
    
    std::stringstream ss;
    
    // 选择器
    for (size_t i = 0; i < m_selectors.size(); ++i) {
        if (i > 0) {
            ss << ", ";
        }
        ss << m_selectors[i]->toCSS();
    }
    
    ss << " {\n";
    
    // 属性
    for (const auto& property : m_properties) {
        ss << "  " << property->toCSS() << "\n";
    }
    
    ss << "}";
    
    return ss.str();
}

bool StyleRule::isEmpty() const {
    return m_selectors.empty() || m_properties.empty();
}

// StyleNode implementation
StyleNode::StyleNode() : BaseNode(NodeType::STYLE), m_isLocal(false) {}

void StyleNode::addRule(std::unique_ptr<StyleRule> rule) {
    m_rules.push_back(std::move(rule));
}

void StyleNode::addInlineProperty(const std::string& name, const std::string& value) {
    m_inlineProperties[name] = value;
}

std::string StyleNode::getInlineProperty(const std::string& name) const {
    auto it = m_inlineProperties.find(name);
    return it != m_inlineProperties.end() ? it->second : "";
}

bool StyleNode::hasInlineProperty(const std::string& name) const {
    return m_inlineProperties.find(name) != m_inlineProperties.end();
}

bool StyleNode::removeInlineProperty(const std::string& name) {
    auto it = m_inlineProperties.find(name);
    if (it != m_inlineProperties.end()) {
        m_inlineProperties.erase(it);
        return true;
    }
    return false;
}

std::string StyleNode::toCSS() const {
    std::stringstream ss;
    
    for (const auto& rule : m_rules) {
        ss << rule->toCSS() << "\n";
    }
    
    return ss.str();
}

std::string StyleNode::toInlineCSS() const {
    if (m_inlineProperties.empty()) {
        return "";
    }
    
    std::stringstream ss;
    bool first = true;
    
    for (const auto& pair : m_inlineProperties) {
        if (!first) {
            ss << "; ";
        }
        ss << pair.first << ": " << pair.second;
        first = false;
    }
    
    return ss.str();
}

std::unique_ptr<BaseNode> StyleNode::clone() const {
    auto cloned = std::make_unique<StyleNode>();
    cloned->m_isLocal = m_isLocal;
    cloned->m_inlineProperties = m_inlineProperties;
    cloned->m_line = m_line;
    cloned->m_column = m_column;
    
    if (m_token) {
        cloned->m_token = std::make_unique<Token>(*m_token);
    }
    
    for (const auto& rule : m_rules) {
        // 简化克隆，实际应该深度克隆规则
        cloned->m_rules.push_back(std::make_unique<StyleRule>());
    }
    
    return cloned;
}

void StyleNode::accept(NodeVisitor* visitor) {
    if (visitor) {
        visitor->visitStyle(this);
    }
}

std::string StyleNode::toString() const {
    std::stringstream ss;
    ss << "StyleNode(";
    if (m_isLocal) {
        ss << "local";
    } else {
        ss << "global";
    }
    ss << ", " << m_rules.size() << " rules";
    if (!m_inlineProperties.empty()) {
        ss << ", " << m_inlineProperties.size() << " inline properties";
    }
    ss << ")";
    return ss.str();
}

bool StyleNode::isValidPropertyName(const std::string& name) {
    // 简化的属性名验证
    if (name.empty()) return false;
    
    // 检查是否包含有效字符
    for (char c : name) {
        if (!std::isalnum(c) && c != '-' && c != '_') {
            return false;
        }
    }
    
    return true;
}

bool StyleNode::isValidPropertyValue(const std::string& value) {
    // 简化的属性值验证
    return !value.empty();
}

std::string StyleNode::formatPropertyName(const std::string& name) {
    std::string result = name;
    
    // 将下划线转换为连字符
    std::replace(result.begin(), result.end(), '_', '-');
    
    return result;
}

std::string StyleNode::formatPropertyValue(const std::string& value) {
    // 简化的属性值格式化
    return value;
}

void StyleNode::initializeCSSProperties() {
    // 初始化CSS属性验证规则
    // 这里可以添加更复杂的验证逻辑
}

} // namespace CHTL