#ifndef GENERATOR_H
#define GENERATOR_H

#include "../CHTLNode/AST.h"
#include <string>
#include <sstream>
#include <set>
#include <map>

// Forward declare nodes for the symbol table
class TemplateStyleNode;
class TemplateElementNode;

// A visitor to specifically find and collect template definitions into symbol tables.
class TemplateCollector : public Visitor {
public:
    TemplateCollector(
        std::map<std::string, TemplateStyleNode*>& st,
        std::map<std::string, TemplateElementNode*>& et
    );

    void collect(ProgramNode& root);

    // We only care about top-level declarations, so most visit methods are empty.
    void visit(ProgramNode& node) override;
    void visit(ElementNode& node) override;
    void visit(AttributeNode& node) override {}
    void visit(TextNode& node) override {}
    void visit(CommentNode& node) override {}
    void visit(StringLiteralNode& node) override {}
    void visit(UnquotedLiteralNode& node) override {}
    void visit(NumberLiteralNode& node) override {}
    void visit(StyleBlockNode& node) override {}
    void visit(StylePropertyNode& node) override {}
    void visit(StyleRuleNode& node) override {}
    void visit(UseStyleNode& node) override {}
    void visit(UseElementNode& node) override {}
    void visit(BinaryOpNode& node) override {}
    void visit(DimensionNode& node) override {}

    // These are the methods that will actually do the work.
    void visit(TemplateStyleNode& node) override;
    void visit(TemplateElementNode& node) override;

private:
    std::map<std::string, TemplateStyleNode*>& style_templates;
    std::map<std::string, TemplateElementNode*>& element_templates;
};


class Generator : public Visitor {
public:
    Generator();

    // Main entry point to generate HTML code from an AST.
    std::string generateHTML(ProgramNode& root);

    // Gets the CSS code generated from style rules.
    std::string getCSS();

    // Visitor methods for each node type.
    void visit(ProgramNode& node) override;
    void visit(ElementNode& node) override;
    void visit(AttributeNode& node) override;
    void visit(TextNode& node) override;
    void visit(CommentNode& node) override;
    void visit(StringLiteralNode& node) override;
    void visit(UnquotedLiteralNode& node) override;
    void visit(NumberLiteralNode& node) override;
    void visit(StyleBlockNode& node) override;
    void visit(StylePropertyNode& node) override;
    void visit(StyleRuleNode& node) override;
    void visit(TemplateStyleNode& node) override;
    void visit(TemplateElementNode& node) override;
    void visit(UseStyleNode& node) override;
    void visit(UseElementNode& node) override;
    void visit(BinaryOpNode& node) override;
    void visit(DimensionNode& node) override;

private:
    enum class StyleContext { INLINE, GLOBAL_RULE };

    // Symbol Tables for template definitions
    std::map<std::string, TemplateStyleNode*> style_templates;
    std::map<std::string, TemplateElementNode*> element_templates;

    std::stringstream html_buffer;
    std::stringstream css_buffer;
    std::stringstream value_buffer;

    StyleContext style_context = StyleContext::INLINE;
    int indent_level = 0;

    static const std::set<std::string> void_elements;

    void do_indent();
    void generateTokenSequence(std::stringstream& buffer, const std::vector<Token>& tokens);
};

#endif // GENERATOR_H
