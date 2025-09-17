#pragma once

#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/RootNode.h"
#include "../CHTLNode/RuleNode.h"
#include <string>
#include <vector>
#include <memory>
#include <sstream>

class Generator : public AstVisitor {
public:
    std::string generate(std::shared_ptr<RootNode> ast);

    void visit(RootNode& node) override;
    void visit(ElementNode& node) override;
    void visit(AttributeNode& node) override;
    void visit(TextNode& node) override;
    void visit(StyleNode& node) override;
    void visit(PropertyNode& node) override;
    void visit(RuleNode& node) override;

private:
    std::string html;
    std::stringstream globalCss;
    int indentLevel = 0;

    void indent();
};
