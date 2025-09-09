#include "CHTLJS/CHTLJSState.h"

namespace CHTL {

CHTLJSState::CHTLJSState() {
}

CHTLJSState::~CHTLJSState() = default;

void CHTLJSState::setCurrentFunction(const std::string& functionName) {
    m_currentFunction = functionName;
}

std::string CHTLJSState::getCurrentFunction() const {
    return m_currentFunction;
}

} // namespace CHTL