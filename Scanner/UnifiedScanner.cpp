#include "UnifiedScanner.h"
#include <iostream>

namespace CHTL {

UnifiedScanner::UnifiedScanner(const std::string& source)
    : m_source(source) {}

void UnifiedScanner::scan() {
    // This is the entry point for the state machine.
    // The actual implementation will be a complex state machine that
    // parses the source and identifies different code blocks based on keywords
    // like 'style', 'script', '[Origin]', etc., and context (e.g., inside a
    // script block).

    // For now, this is a placeholder implementation that treats the entire
    // source file as a single CHTL fragment. This allows the rest of the
    // compiler pipeline to be developed.
    if (m_fragments.empty() && !m_source.empty()) {
        m_fragments.push_back({CodeType::CHTL, m_source});
    }

    std::cout << "Note: UnifiedScanner::scan() is using a placeholder implementation." << std::endl;
}

const std::vector<CodeFragment>& UnifiedScanner::getFragments() const {
    return m_fragments;
}

bool UnifiedScanner::isEOF() const {
    return m_cursor >= m_source.length();
}

char UnifiedScanner::peek() const {
    if (isEOF()) return '\0';
    return m_source[m_cursor];
}

void UnifiedScanner::advance() {
    if (!isEOF()) {
        m_cursor++;
    }
}

void UnifiedScanner::scanRoot() {
    // The real implementation will start here, iterating through the source
    // and transitioning between states based on the characters it encounters.
    // For example, it would look for keywords to enter a script or style scanning state.
}

} // namespace CHTL
