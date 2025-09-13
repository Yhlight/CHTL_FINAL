#ifndef CHTL_STYLE_TEMPLATE_NODE_H
#define CHTL_STYLE_TEMPLATE_NODE_H

#include <string>
#include <vector>
#include <map>
#include <unordered_set>
#include <memory>
#include "ExpressionNode.h"

namespace CHTL {

struct SpecializationInfo {
    std::unordered_set<std::string> deleted_properties;
    std::unordered_set<std::string> deleted_templates;
};

// This is a storage class for the parser, not a part of the main AST tree.
class StyleTemplateNode {
public:
    explicit StyleTemplateNode(const std::string& name, bool is_custom = false)
        : name_(name), is_custom_(is_custom) {}

    std::string name_;
    bool is_custom_;
    std::vector<std::string> inherits_;
    std::map<std::string, SpecializationInfo> inheritance_specializations_;
    // Placeholders are represented by a null ExpressionNode
    std::vector<std::pair<std::string, std::unique_ptr<ExpressionNode>>> properties_;
};

} // namespace CHTL

#endif // CHTL_STYLE_TEMPLATE_NODE_H
