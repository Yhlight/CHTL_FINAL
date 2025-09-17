#ifndef GENERATOR_H
#define GENERATOR_H

#include "../CHTLNode/Node.h"
#include <string>
#include <sstream>

namespace CHTL {

class Generator {
public:
    Generator();

    // The main entry point to generate the HTML body from an AST.
    std::string generate(const Node* rootNode);
    // Gets the CSS collected from CSS rule blocks.
    std::string getGlobalCss() const;

private:
    // Main dispatch function that calls the appropriate specific generator.
    void visit(const Node* node);

    // Specific visitor functions for each node type.
    void visitRootNode(const RootNode* node);
    void visitElementNode(const ElementNode* node);
    void visitTextNode(const TextNode* node);
    void visitStyleNode(const StyleNode* node);
    void visitCssRuleNode(const CssRuleNode* node);

    // Helper for managing indentation to produce readable HTML.
    void writeIndent();

    std::stringstream m_ss; // String stream to build the HTML body
    std::stringstream m_global_css_stream; // Stream for collecting global CSS rules
    int m_indent_level = 0;
};

} // namespace CHTL

#endif // GENERATOR_H
