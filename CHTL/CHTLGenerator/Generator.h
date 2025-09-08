#pragma once

#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/TemplateUsageNode.h"
#include <string>
#include <sstream>

class Parser;

class Generator : public AstVisitor {
public:
    Generator(Parser& parser);
    std::string generate(ElementNode& root);

    void visit(ElementNode& node) override;
    void visit(TextNode& node) override;
    void visit(StyleNode& node) override;
    void visit(TemplateUsageNode& node) override;
    void visit(class CustomStyleTemplateNode& node) override;
    void visit(class StyleRuleNode& node) override;

private:
    Parser& parser;
    std::stringstream output;
    std::stringstream global_css;
};
