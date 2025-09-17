#include "CHTLGenerator.h"
#include <stdexcept>

namespace CHTL {

CHTLGenerator::CHTLGenerator() : m_indent_level(0) {}

std::string CHTLGenerator::Generate(Program* program) {
    m_output.str(""); // Clear the stream
    m_indent_level = 0;
    for (const auto& stmt : program->statements) {
        generateNode(stmt.get());
    }
    return m_output.str();
}

void CHTLGenerator::indent() {
    for (int i = 0; i < m_indent_level; ++i) {
        m_output << "  "; // 2 spaces for indentation
    }
}

void CHTLGenerator::generateNode(BaseNode* node) {
    if (auto* element = dynamic_cast<ElementNode*>(node)) {
        generateElementNode(element);
    } else if (auto* text = dynamic_cast<TextNode*>(node)) {
        generateTextNode(text);
    } else if (auto* comment = dynamic_cast<CommentNode*>(node)) {
        generateCommentNode(comment);
    }
    // Note: Other node types will be added here
}

void CHTLGenerator::generateElementNode(ElementNode* node) {
    indent();
    m_output << "<" << node->GetTokenLiteral() << ">" << "\n";

    m_indent_level++;
    for (const auto& child : node->children) {
        generateNode(child.get());
    }
    m_indent_level--;

    indent();
    m_output << "</" << node->GetTokenLiteral() << ">" << "\n";
}

void CHTLGenerator::generateTextNode(TextNode* node) {
    indent();
    m_output << node->GetTokenLiteral() << "\n";
}

void CHTLGenerator::generateCommentNode(CommentNode* node) {
    indent();
    // Assuming the token literal is like "# some comment"
    // We want to extract " some comment"
    std::string content = node->GetTokenLiteral();
    if (content.rfind("# ", 0) == 0) {
        content = content.substr(2);
    } else if (content.rfind("#", 0) == 0) {
        content = content.substr(1);
    }
    m_output << "<!-- " << content << " -->" << "\n";
}

} // namespace CHTL
