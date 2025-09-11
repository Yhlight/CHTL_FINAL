#pragma once

#include "Node.h"
#include <string>
#include <vector>
#include <sstream>

namespace CHTL {

enum class TemplateType {
    STYLE,
    ELEMENT,
    VAR
};

class TemplateDefinitionNode : public Node {
public:
    TemplateDefinitionNode(TemplateType type, std::string name, NodeList body)
        : template_type(type), name(std::move(name)), body(std::move(body)) {}

    std::string ToString(int indent = 0) const override {
        std::stringstream ss;
        ss << std::string(indent, ' ') << "TemplateDefinition( " << name << " ) [\n";
        for (const auto& node : body) {
            ss << node->ToString(indent + 2) << "\n";
        }
        ss << std::string(indent, ' ') << "]";
        return ss.str();
    }

    TemplateType template_type;
    std::string name;
    NodeList body;
};

} // namespace CHTL
