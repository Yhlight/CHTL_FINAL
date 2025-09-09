#include "ScriptNode.h"

namespace CHTL {

ScriptNode::ScriptNode() : BaseNode(NodeType::SCRIPT), m_isLocal(false) {}

void ScriptNode::appendContent(const std::string& content) {
    m_content += content;
}

bool ScriptNode::isEmpty() const {
    return m_content.empty();
}

std::unique_ptr<BaseNode> ScriptNode::clone() const {
    auto cloned = std::make_unique<ScriptNode>();
    cloned->m_content = m_content;
    cloned->m_isLocal = m_isLocal;
    cloned->m_line = m_line;
    cloned->m_column = m_column;
    
    if (m_token) {
        cloned->m_token = std::make_unique<Token>(*m_token);
    }
    
    return cloned;
}

void ScriptNode::accept(NodeVisitor* visitor) {
    if (visitor) {
        visitor->visitScript(this);
    }
}

std::string ScriptNode::toString() const {
    std::stringstream ss;
    ss << "ScriptNode(\"" << m_content << "\"";
    if (m_isLocal) {
        ss << ", local";
    }
    ss << ")";
    return ss.str();
}

std::string ScriptNode::toHTML() const {
    if (m_content.empty()) {
        return "";
    }
    
    std::stringstream ss;
    ss << "<script>";
    if (m_isLocal) {
        ss << "\n// 局部脚本\n";
    }
    ss << m_content;
    ss << "</script>";
    return ss.str();
}

std::string ScriptNode::toJS() const {
    return m_content;
}

} // namespace CHTL