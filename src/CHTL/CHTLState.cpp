#include "CHTL/CHTLState.h"

namespace CHTL {

CHTLState::CHTLState() : m_currentLine(1), m_currentColumn(1) {
}

CHTLState::~CHTLState() = default;

void CHTLState::setPosition(size_t line, size_t column) {
    m_currentLine = line;
    m_currentColumn = column;
}

size_t CHTLState::getCurrentLine() const {
    return m_currentLine;
}

size_t CHTLState::getCurrentColumn() const {
    return m_currentColumn;
}

void CHTLState::pushScope() {
    m_scopeStack.push_back({});
}

void CHTLState::popScope() {
    if (!m_scopeStack.empty()) {
        m_scopeStack.pop_back();
    }
}

void CHTLState::addVariable(const std::string& name, const std::string& value) {
    if (!m_scopeStack.empty()) {
        m_scopeStack.back()[name] = value;
    }
}

std::string CHTLState::getVariable(const std::string& name) const {
    // 从当前作用域开始向上查找
    for (auto it = m_scopeStack.rbegin(); it != m_scopeStack.rend(); ++it) {
        auto varIt = it->find(name);
        if (varIt != it->end()) {
            return varIt->second;
        }
    }
    return "";
}

bool CHTLState::hasVariable(const std::string& name) const {
    return !getVariable(name).empty();
}

void CHTLState::clear() {
    m_scopeStack.clear();
    m_currentLine = 1;
    m_currentColumn = 1;
}

} // namespace CHTL