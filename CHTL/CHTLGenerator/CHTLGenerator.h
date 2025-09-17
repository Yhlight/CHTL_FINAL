#pragma once

#include "../CHTLNode/Node.h"
#include <string>
#include <sstream>

class CHTLGenerator : public INodeVisitor {
public:
    CHTLGenerator();

    std::string generate(Node& root);

    void visit(ProgramNode& node) override;
    void visit(ElementNode& node) override;
    void visit(AttributeNode& node) override;
    void visit(TextNode& node) override;
    void visit(StyleNode& node) override;
    void visit(SelectorNode& node) override;

private:
    std::string generateExpression(ExpressionNode* node, bool is_top_level = true);

    std::stringstream html_out;
    std::stringstream css_out;

    // A context flag to know if we are inside a style block
    bool in_style_block = false;
    // The selector of the element currently being processed (for '&' replacement)
    std::string current_element_selector;

    // Helper to clean up string literals (remove quotes)
    std::string unquote(const std::string& s);
};
