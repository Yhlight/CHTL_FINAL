#ifndef TEMPLATE_DATA_H
#define TEMPLATE_DATA_H

#include "../CHTLNode/BaseNode.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <utility>
#include <memory>
#include <optional>

namespace CHTL {

// --- Data Structures for [Template] ---

struct StyleTemplate {
    std::string name;
    std::vector<std::pair<std::string, std::string>> properties;
    std::vector<std::string> inherited_templates;
};

struct ElementTemplate {
    std::string name;
    std::vector<std::unique_ptr<BaseNode>> nodes;
};

struct VarTemplate {
    std::string name;
    std::unordered_map<std::string, std::string> variables;
};


// --- Data Structures for [Custom] ---

struct CustomStyleProperty {
    std::string name;
    std::optional<std::string> value;
};

struct CustomStyleTemplate {
    std::string name;
    std::vector<CustomStyleProperty> properties;
    std::vector<std::string> inherited_templates;
    std::vector<std::string> deleted_keys;
};

// The definition of a Custom Element is the same as a Template Element.
// The "custom" logic happens at the usage site.
struct CustomElementTemplate {
    std::string name;
    std::vector<std::unique_ptr<BaseNode>> nodes;
};


} // namespace CHTL

#endif // TEMPLATE_DATA_H
