#pragma once

#include "Node.h"
#include "TemplateDefinitionNode.h" // For TemplateType enum
#include <string>
#include <utility>

namespace CHTL {

class TemplateUsageNode : public Node {
public:
    TemplateUsageNode(TemplateType type, std::string name)
        : template_type(type), name(std::move(name)) {}

    std::string ToString(int indent = 0) const override {
        return std::string(indent, ' ') + "TemplateUsage( @" + name + " )";
    }

    TemplateType template_type;
    std::string name;
};

} // namespace CHTL
