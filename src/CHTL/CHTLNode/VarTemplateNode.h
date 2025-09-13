#ifndef CHTL_VAR_TEMPLATE_NODE_H
#define CHTL_VAR_TEMPLATE_NODE_H

#include <string>
#include <vector>
#include <unordered_map>
#include "PropertyValue.h"

namespace CHTL {

// This is a storage class for the parser, not a part of the main AST tree.
class VarTemplateNode {
public:
    explicit VarTemplateNode(const std::string& name) : name_(name) {}

    std::string name_;
    std::vector<std::string> inherits_;
    std::unordered_map<std::string, std::vector<PropertyValue>> variables_;
};

} // namespace CHTL

#endif // CHTL_VAR_TEMPLATE_NODE_H
