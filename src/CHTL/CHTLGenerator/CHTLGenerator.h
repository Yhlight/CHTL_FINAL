#pragma once

#include "BaseNode.h"
#include "ElementNode.h"
#include "TextNode.h"
#include <string>
#include <memory>
#include <sstream>

namespace CHTL {

class CHTLGenerator {
public:
    explicit CHTLGenerator(std::shared_ptr<ElementNode> root);

    std::string generate();

private:
    std::shared_ptr<ElementNode> root;
    std::stringstream output;
    int indentLevel = 0;

    void generateNode(const std::shared_ptr<BaseNode>& node);
    void generateElement(const std::shared_ptr<ElementNode>& node);
    void generateText(const std::shared_ptr<TextNode>& node);

    void indent();
};

} // namespace CHTL
