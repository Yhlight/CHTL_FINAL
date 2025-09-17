#pragma once

#include "../CHTLNode/Node.h"
#include <string>
#include <sstream>
#include <map>

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
    void visit(TemplateDefinitionNode& node) override;
    void visit(TemplateUsageNode& node) override;
    void visit(DeleteNode& node) override;

private:
    std::string generateExpression(ExpressionNode* node, bool is_top_level = true);
    void collectTemplateDefinitions(Node* node);

    std::map<std::string, TemplateDefinitionNode*> template_table;
    std::stringstream html_out;
    std::stringstream css_out;

    // A context flag to know if we are inside a style block
    bool in_style_block = false;
    // The selector of the element currently being processed (for '&' replacement)
    std::string current_element_selector;
    // A context flag to know if we are generating for an inline style attribute
    bool generating_inline_style = false;
    // The stream to write CSS to (can be global css_out or a temporary stream)
    std::stringstream* active_css_stream = &css_out;

    // Helper to clean up string literals (remove quotes)
    std::string unquote(const std::string& s);
};
