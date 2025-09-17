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
    void visit(ValueNode& node) override;

private:
    std::stringstream html_out;
    std::stringstream css_out;

    // A context flag to know if we are inside a style block
    bool in_style_block = false;

    // Helper to clean up string literals (remove quotes)
    std::string unquote(const std::string& s);
};
