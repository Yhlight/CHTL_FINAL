#ifndef CHTL_STYLE_TEMPLATE_NODE_H
#define CHTL_STYLE_TEMPLATE_NODE_H

#include <string>
#include <vector>
#include <utility>
#include <map>
#include <unordered_set>
#include "PropertyValue.h"

namespace CHTL {

// This is a storage class for the parser, not a part of the main AST tree.
class StyleTemplateNode {
public:
    explicit StyleTemplateNode(const std::string& name, bool is_custom = false)
        : name_(name), is_custom_(is_custom) {}

    std::string name_;
    bool is_custom_;
    std::vector<std::string> inherits_;
    // e.g. from "ParentTemplate" -> set of {"prop1", "prop2"}
    std::map<std::string, std::unordered_set<std::string>> inheritance_specializations_;
    std::vector<std::pair<std::string, std::vector<PropertyValue>>> properties_;
};

} // namespace CHTL

#endif // CHTL_STYLE_TEMPLATE_NODE_H
