#pragma once

#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/RootNode.h"
#include <string>
#include <memory>

class Generator : public AstVisitor {
public:
    std::string generate(std::shared_ptr<RootNode> ast);

    void visit(RootNode& node) override;
    void visit(ElementNode& node) override;
    void visit(AttributeNode& node) override;
    void visit(TextNode& node) override;

private:
    std::string html;
    int indentLevel = 0;
    bool isFirstAttribute = true;

    void indent();
};
