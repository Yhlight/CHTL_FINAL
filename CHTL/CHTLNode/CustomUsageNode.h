#pragma once

#include "Node.h"
#include "TemplateDefinitionNode.h" // For TemplateType enum
#include <string>
#include <utility>
#include <vector>

namespace CHTL {

class CustomUsageNode : public Node {
public:
    // The body will contain the specialization statements (delete, insert etc.)
    CustomUsageNode(TemplateType type, std::string name, NodeList body)
        : template_type(type), name(std::move(name)), body(std::move(body)) {}

    std::string ToString(int indent = 0) const override {
        return std::string(indent, ' ') + "CustomUsage( @" + name + " )";
    }

    NodePtr clone() const override {
        NodeList new_body;
        for (const auto& node : body) {
            new_body.push_back(node->clone());
        }
        return std::make_shared<CustomUsageNode>(template_type, name, new_body);
    }

    TemplateType template_type;
    std::string name;
    NodeList body;
};

} // namespace CHTL
