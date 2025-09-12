#ifndef CHTL_ELEMENT_TEMPLATE_NODE_H
#define CHTL_ELEMENT_TEMPLATE_NODE_H

#include "BaseNode.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

// This is a storage class for the parser, not a part of the main AST tree.
class ElementTemplateNode {
public:
    explicit ElementTemplateNode(const std::string& name) : name_(name) {}

    std::string name_;
    std::vector<std::unique_ptr<Node>> children_;
};

} // namespace CHTL

#endif // CHTL_ELEMENT_TEMPLATE_NODE_H
