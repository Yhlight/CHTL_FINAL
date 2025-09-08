#ifndef CHTL_GENERATOR_H
#define CHTL_GENERATOR_H

#include "../CHTLNode/BaseNode.h"
#include "../CHTLContext/CHTLContext.h" // Added
#include <string>
#include <vector>

namespace CHTL {

// Forward declarations for node types
class ElementNode;
class TextNode;
class CommentNode;
class StyleNode;
class TemplateUsageNode; // Added

class CHTLGenerator {
public:
    // Updated constructor
    CHTLGenerator(CHTLContext& context);

    std::string generate(BaseNode& root); // Changed to non-const

private:
    void visit(BaseNode& node); // Changed to non-const
    void visitElement(ElementNode& node);
    void visitText(const TextNode& node);
    void visitComment(const CommentNode& node);
    void visitStyleNode(const StyleNode& node, ElementNode& parent);
    void visitTemplateUsage(TemplateUsageNode& node); // Added

    std::string getIndent();

    CHTLContext& m_context; // Added
    std::string m_output;
    std::string m_globalCss;
    int m_indentLevel;
};

} // namespace CHTL

#endif // CHTL_GENERATOR_H
