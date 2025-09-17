#ifndef GENERATOR_H
#define GENERATOR_H

#include "../CHTLNode/AST.h"
#include <string>
#include <sstream>
#include <set>

class Generator : public Visitor {
public:
    Generator();

    // Main entry point to generate code from an AST.
    std::string generate(ProgramNode& root);

    // Visitor methods for each node type.
    void visit(ProgramNode& node) override;
    void visit(ElementNode& node) override;
    void visit(AttributeNode& node) override;
    void visit(TextNode& node) override;
    void visit(CommentNode& node) override;
    void visit(StringLiteralNode& node) override;
    void visit(UnquotedLiteralNode& node) override;
    void visit(NumberLiteralNode& node) override;

private:
    std::stringstream html_buffer;
    // A separate buffer for CSS, which will be used in later steps.
    std::stringstream css_buffer;

    // A temporary buffer to hold the processed value of a ValueNode,
    // which is then used by its parent (e.g., AttributeNode or TextNode).
    std::stringstream value_buffer;

    int indent_level = 0;

    // A set of HTML void elements (tags that don't need a closing tag).
    static const std::set<std::string> void_elements;

    void do_indent();
};

#endif // GENERATOR_H
