#include "CHTLJS/CHTLJSNode/CHTLJSVirtualNode.h"

namespace CHTL {

CHTLJSVirtualNode::CHTLJSVirtualNode() {
}

CHTLJSVirtualNode::~CHTLJSVirtualNode() = default;

std::string CHTLJSVirtualNode::getVirtualName() const {
    return m_virtualName;
}

void CHTLJSVirtualNode::setVirtualName(const std::string& virtualName) {
    m_virtualName = virtualName;
}

} // namespace CHTL