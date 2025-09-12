#pragma once

#include "TemplateDefinitionNode.h"
#include <string>
#include <vector>
#include <sstream>

namespace CHTL {

class CustomDefinitionNode : public TemplateDefinitionNode {
public:
    CustomDefinitionNode(TemplateType type, std::string name, NodeList body)
        : TemplateDefinitionNode(type, name, body) {}

    // Override ToString to show it's a Custom one
    std::string ToString(int indent = 0) const override {
        std::stringstream ss;
        ss << std::string(indent, ' ') << "CustomDefinition( " << name << " ) [\n";
        for (const auto& node : body) {
            ss << node->ToString(indent + 2) << "\n";
        }
        ss << std::string(indent, ' ') << "]";
        return ss.str();
    }

    NodePtr clone() const override {
        NodeList new_body;
        for (const auto& node : body) {
            new_body.push_back(node->clone());
        }
        // It's okay to return a CustomDefinitionNode as a NodePtr
        return std::make_shared<CustomDefinitionNode>(template_type, name, new_body);
    }
};

} // namespace CHTL
