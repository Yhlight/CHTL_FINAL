#pragma once

#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include <string>
#include <sstream>

class Generator : public AstVisitor {
public:
    std::string generate(ElementNode& root);

    void visit(ElementNode& node) override;
    void visit(TextNode& node) override;

private:
    std::stringstream output;
};
