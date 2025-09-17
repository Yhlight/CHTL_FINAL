#pragma once

#include "../CHTLNode/Node.h"
#include <string>
#include <memory>
#include <sstream>

class Generator {
public:
    Generator();
    std::string generate(const std::shared_ptr<RootNode>& ast);

private:
    void visit(const std::shared_ptr<Node>& node);
    void visitElement(const std::shared_ptr<ElementNode>& node);
    void visitText(const std::shared_ptr<TextNode>& node);
    void visitAttribute(const std::shared_ptr<AttributeNode>& node); // This might not be needed if handled within visitElement

    std::stringstream output;
    int indentLevel = 0;
    void printIndent();
};
