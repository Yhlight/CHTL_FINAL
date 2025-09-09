#include "CHTLJS/CHTLJSNode/CHTLJSBaseNode.h"

namespace CHTL {

CHTLJSBaseNode::CHTLJSBaseNode() {
}

CHTLJSBaseNode::~CHTLJSBaseNode() = default;

std::string CHTLJSBaseNode::getNodeType() const {
    return m_nodeType;
}

void CHTLJSBaseNode::setNodeType(const std::string& nodeType) {
    m_nodeType = nodeType;
}

} // namespace CHTL