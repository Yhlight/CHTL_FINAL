#include "StyleNode.h"
#include <sstream>

namespace CHTL {

StyleNode::StyleNode() 
    : BaseNode(NodeType::STYLE), m_styleType(StyleType::INLINE) {
}

void StyleNode::addStyleProperty(const std::string& property, const std::string& value) {
    m_styleProperties[property] = value;
}

const std::string& StyleNode::getStyleProperty(const std::string& property) const {
    static const std::string empty;
    auto it = m_styleProperties.find(property);
    return (it != m_styleProperties.end()) ? it->second : empty;
}

std::shared_ptr<BaseNode> StyleNode::clone() const {
    auto cloned = std::make_shared<StyleNode>();
    cloned->m_styleType = m_styleType;
    cloned->m_selector = m_selector;
    cloned->m_styleProperties = m_styleProperties;
    cloned->m_value = m_value;
    cloned->m_line = m_line;
    cloned->m_column = m_column;
    
    // 克隆子节点
    for (const auto& child : m_children) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

std::string StyleNode::toString() const {
    if (isInline()) {
        std::stringstream ss;
        for (const auto& prop : m_styleProperties) {
            if (!ss.str().empty()) ss << "; ";
            ss << prop.first << ": " << prop.second;
        }
        return ss.str();
    } else {
        return toCSS();
    }
}

std::string StyleNode::toCSS() const {
    std::stringstream css;
    
    if (isSelector()) {
        css << m_selector << " {\n";
        for (const auto& prop : m_styleProperties) {
            css << "    " << prop.first << ": " << prop.second << ";\n";
        }
        css << "}";
    } else {
        // 内联样式
        for (const auto& prop : m_styleProperties) {
            if (!css.str().empty()) css << "; ";
            css << prop.first << ": " << prop.second;
        }
    }
    
    return css.str();
}

void StyleNode::accept(NodeVisitor& visitor) {
    visitor.visitStyle(*this);
}

} // namespace CHTL