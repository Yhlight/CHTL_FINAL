#include "CHTLJS/CHTLJSNode/CHTLJSFunctionNode.h"

namespace CHTL {

CHTLJSFunctionNode::CHTLJSFunctionNode() {
}

CHTLJSFunctionNode::~CHTLJSFunctionNode() = default;

std::string CHTLJSFunctionNode::getFunctionName() const {
    return m_functionName;
}

void CHTLJSFunctionNode::setFunctionName(const std::string& functionName) {
    m_functionName = functionName;
}

} // namespace CHTL