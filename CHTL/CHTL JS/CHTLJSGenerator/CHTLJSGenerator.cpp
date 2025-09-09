#include "CHTLJSGenerator.h"

namespace CHTL {
namespace CHTL_JS {

CHTLJSGenerator::CHTLJSGenerator() {}

std::string CHTLJSGenerator::generate(CHTLJSProgram* program) {
    m_out.str("");
    m_out.clear();
    visit(program);
    return m_out.str();
}

void CHTLJSGenerator::visit(CHTLJSNode* node) {
    if (auto p = dynamic_cast<CHTLJSProgram*>(node)) { visit(p); }
    else if (auto es = dynamic_cast<EnhancedSelectorNode*>(node)) { visit(es); }
    else if (auto ln = dynamic_cast<ListenNode*>(node)) { visit(ln); }
    else if (auto rjs = dynamic_cast<RawJavaScriptNode*>(node)) { visit(rjs); }
}

void CHTLJSGenerator::visit(CHTLJSProgram* node) {
    for (const auto& stmt : node->statements) {
        visit(stmt.get());
        m_out << ";\n";
    }
}

void CHTLJSGenerator::visit(EnhancedSelectorNode* node) {
    // This is a simplified translation. A full implementation would handle
    // the different selector types more robustly.
    m_out << "document.querySelector('" << node->selector << "')";
}

void CHTLJSGenerator::visit(ListenNode* node) {
    // The parser is simplified, so the generator is too.
    m_out << "/* Listen block for target would be generated here */";
}

void CHTLJSGenerator::visit(RawJavaScriptNode* node) {
    m_out << node->code;
}

} // namespace CHTL_JS
} // namespace CHTL
