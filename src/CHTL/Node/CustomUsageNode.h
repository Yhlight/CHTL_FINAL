#ifndef CHTL_CUSTOM_USAGE_NODE_H
#define CHTL_CUSTOM_USAGE_NODE_H

#include "BaseNode.h"
#include "TemplateNode.h" // For TemplateType enum
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

// Represents the usage of a Custom or Template block with a specialization body.
// e.g., @Style MyTemplate { delete color; }
class CustomUsageNode : public BaseNode {
public:
    TemplateType usageType; // Re-using TemplateType
    std::string name;
    // The body contains the specialization nodes (e.g., DeleteNode)
    std::vector<std::shared_ptr<BaseNode>> specializationBody;

    std::shared_ptr<BaseNode> clone() const override {
        auto node = std::make_shared<CustomUsageNode>();
        node->usageType = this->usageType;
        node->name = this->name;
        for (const auto& child : this->specializationBody) {
            node->specializationBody.push_back(child->clone());
        }
        return node;
    }
};

} // namespace CHTL

#endif // CHTL_CUSTOM_USAGE_NODE_H
