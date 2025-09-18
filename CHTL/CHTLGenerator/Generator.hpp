#pragma once

#include "../CHTLNode/Node.hpp"
#include <string>

namespace CHTL {

class Generator {
public:
    Generator();
    std::string generate(const Program* program);

private:
    std::string generateNode(const Node* node);
    std::string generateElementNode(const ElementNode* node);
    std::string generateTextNode(const TextNode* node);
};

} // namespace CHTL
